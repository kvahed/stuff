

#!/bin/bash

endpoint=""
reportfile=arangodinspect.json

if [ $# > 2 ] && [ $# == 0 ]; then
    echo "**Error** - usage: arangoinspect.sh [https://]<endpoint> [jwt-secret-file]"
    exit 1
elif [[ $# == 2 ]]; then
    secfile=$2
fi

endpoint=$1

if [ ! -z $secfile ]; then
    if [ -f $secfile ]; then
        authstr="Authorization: bearer $(jwtgen -s $(cat $secfile) -a HS256 -c 'iss=arangodb' -c 'server_id=setup')"
    else
        echo "**Error** - $secfile entailing the JWT secret could not be found"
    fi
    echo "Inspecting encrypted cluster instance $endpoint"
    
else
    echo "Inspecting non-encrypted cluster instance $endpoint "
fi

instance=$(echo -n $(echo $endpoint | md5sum | awk {'print $1'}))
prefix=/tmp/arangoinspect-$instance


if [ ! -z $secfile ]; then 
    response=$(curl -sH "$authstr" --write-out %{http_code} --silent --output $prefix-version.json $endpoint/_api/version?details=true)
    if  [ $response != 200 ]; then
        echo "  ... authentication failure."
        exit 1
    fi
fi

response=$(curl -sH "$authstr" --write-out %{http_code} --silent --output $prefix-status.json $endpoint/_admin/status)
response=$(curl -sH "$authstr" --write-out %{http_code} --silent --output $prefix-statistics.json $endpoint/_admin/statistics)
response=$(curl -sH "$authstr" --write-out %{http_code} --silent --output $prefix-log.json $endpoint/_admin/log)
databases=$(curl -sH "$authstr" $endpoint/_api/database | jq .result | sed 's/\]//;s/\[//;s/\"//g' )
for i in $databases; do
    response=$(curl -sH "$authstr" --write-out %{http_code} --silent --output $prefix-$i.json $endpoint/_db/$i/_api/collection)
done

agent=$(grep -q AGENT $prefix-log.json)
if [ $? -eq 0 ]; then
    instancetype=agent;
    response=$(curl -sH "$authstr" --write-out %{http_code} --silent --output $prefix-config.json $endpoint/_api/agency/config)
    response=$(curl -sH "$authstr" --write-out %{http_code} --silent --output $prefix-state.json $endpoint/_api/agency/state)
    leaderId=$(cat $prefix-config.json | jq .leaderId)
    localId=$(cat $prefix-config.json | jq .configuration.id)
    if  [ $leaderId = $localId ]; then
        response=$(curl -sH "$authstr" -d'[["/"]]' --write-out %{http_code} --silent --output $prefix-agency.json $endpoint/_api/agency/read)
    fi
fi
agent=$(grep -q COORDINATOR $prefix-log.json)
if [ $? -eq 0 ]; then
    instancetype=coordinator
fi
agent=$(grep -q PRIMARY $prefix-log.json)
if [ $? -eq 0 ]; then
    instancetype=primary
fi

uptime > $prefix-uptime.txt
uname -a > $prefix-uname.txt
cat /etc/*-release > $prefix-release.txt
dmesg > $prefix-dmesg.txt
/sbin/ip addr > $prefix-ip.txt
cat /proc/meminfo > $prefix-meminfo.txt
df -h > $prefix-df.txt
date -u "+%Y-%m-%d %H:%M:%S %Z" > $prefix-date.txt
report=$instancetype-$(hostname)-$instance.tgz
tar cpzf $report $prefix* 2> /dev/null

echo "... done. Report is written to $report"
