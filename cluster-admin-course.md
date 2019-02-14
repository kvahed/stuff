# ArangoDB Cluster Administration Guide

## Introduction

* General thoughts
	* Why clustering
		* Data size
		* Redundancy
	* Advantages
		* Many affordable / available machines **work** together
		* Many affordable / available machines **back** each other
	* Disadvantages
		* Additional network traffic will cost **latency**
		* Additional network traffic will cost **resources**

* Nomenclature
	* Agents, Agency
	* Coordinators
	* Database servers 
	* Database
	* Collection vs shard 
	* Synchronous Replication
	* Storage engines

## Anatomy
* Anatomy of a cluster
	* Agents 
		* specific command line parameters
		* warnings
	* Coordinators
		* specific command line parameters
	* Database servers
		* specific command line parameters
		* warnings
	
## Agency in focus
* Consensus
	* RAFT & the agency
	* Why is it so important

## Quick intermezzo
* `dsh` distributed shell, example:

	```bash 
	> cat .dsh/group/arangodb
	  c11
	  c12
	  c13
	> dsh -Mcg arangodb "sudo systemctl start arangodb"
	```
* `cluserssh` 

## Basic administration
* Assuming 3 machines `c11` - `c13`
* Installation, disable single server service
	```bash
	> sudo yum install arangodb3 / sudo apt install arangodb3
	> sudo systemctl stop arangodb3
	> sudo systemctl disable arangodb 3 
	```
* Startup an arangodb cluster, perform on all 3 machines:
	* Create secret file `/etc/arangodb3/secret.txt`
	```bash 
	some-very-secret-secret
	```	
    * Secure secret
	  ```bash
	  sudo chmod 400 /etc/arangodb3/secret.txt
	  sudo chown arangodb:arangodb /etc/arangodb3/secret.txt
	  ```
	* Create `systemd` script for starter (`arangodb-cluster-node.service`):
    ```bash
	[Unit]
	Description=ArangoDB starter
	After=network.target

	[Service]
	Restart=on-failure
	User=arangodb
	Group=arangodb
	#EnvironmentFile=/etc/arangodb.env
	Environment=CLUSTERSECRETPATH=/etc/arangodb3/secret.txt
	Environment=DATADIR=/opt/kaveh/arangodb
	Environment=CLUSTERSECRET=abc
	Environment=STARTERPORT=9000
	Environment=STARTERENDPOINTS=c11:9000,c12:9000,c13:9000
	ExecStartPre=/bin/sh -c "mkdir -p $(dirname $CLUSTERSECRETPATH)"
	ExecStartPre=/bin/sh -c "mkdir -p ${DATADIR}"
	ExecStartPre=/bin/sh -c "echo ${CLUSTERSECRET} > ${CLUSTERSECRETPATH}"
	ExecStart=/usr/bin/arangodb \
		--log.file=false \
		--starter.data-dir=${DATADIR} \
		--starter.join=${STARTERENDPOINTS} \
		--starter.port=${STARTERPORT} \
		--auth.jwt-secret=${CLUSTERSECRETPATH} \
		--all.log.level=info \
		--all.log.output=+
	TimeoutStopSec=60

	[Install]
	WantedBy=multi-user.target
	```
	```bash
	> dsh -Mcg arangodb sudo cp arangodb-cluster-node.service /lib/system/systemd/
	> dsh -Mcg arangodb sudo cp systemctl daemon-reload
	> dsh -Mcg arangodb sudo systemctl enable arangodb-cluster-node
	> dsh -Mcg arangodb sudo systemctl start arangodb-cluster-node 	
	```
* Shutdown / restart an arangodb cluster
	```bash
	> dsh -Mcg arangodb sudo systemctl [stop/restart] arangodb-cluster-node
	``` 
* Authorization header all calls to REST APIs 
	```bash
	> AUTH_HEADER=$(sudo arangodb auth header --auth.jwt-secret /etc/arangodb3/secret.txt)
	```
* Create / drop Databases (same as single server)
	* UI
	* Rest API
	```bash
	curl -sH"$AUTH_HEADER" c11:9001/_api/database -d '{"name":"foo"}'
	```
	* `arangosh`
	```bash
	db._createDatabase("foo")
	```
* Create / drop Collections are the exception, where you have control over data distribution (`numberOfShards`) and resilience (`replicationFactor`) 
	```bash
	> curl -H"$AUTH_HEADER" c11:9001/_api/collection \
		-d '{"name":"bar", "replicationFactor":2, "numberOfShards":15}' 
	``` 
	```bash
	> db._create("bar", {"replicationFactor":2, "numberOfShards":15});
	```
* Ensure / drop Indexes 
* User administration

## Advanced administration
* **Supervision** pay attention to `Status` below. This API could be used for regular monitoring
	```bash
	>curl -sH"$AUTH_HEADER" c11:9001/_admin/cluster/health
	```
	```javascript
	{ 
	  "Health": {
	    "PRMR-43e7e362-86b8-4c00-b7b2-588da91b5f7d": {
	      "Endpoint":"tcp://192.168.10.12:9002",
	      "LastAckedTime": "2019-02-14T07:38:46Z",
	      "SyncTime": "2019-02-14T07:38:46Z",
	      "Version": "3.4.2-1",
	      "Engine": "rocksdb",
	      "ShortName": "DBServer0001",
	      "Timestamp": "2019-02-14T07:38:47Z",
	      "Status": "GOOD",
	      "SyncStatus": "SERVING",
	      "Host": "d47cee4dfa98b22f2236e519594ab452",
	      "Role": "DBServer",
          "CanBeDeleted": false
        }, 
        // and the rest of them, agents and coordinators
      }
    }
	```
* ArangoDB **starter** [more in depth](https://github.com/arangodb-helper/arangodb/tree/master/docs/Manual/Tutorials/Starter) 
	* OS level
	* Docker
	* SSL
	* Authentication
* **ArangoSync** 
	```bash
	> cd /tmp
    > mkdir a b certs
    # Certificates
    > cd /tmp/certs
    > arangodb create tls ca
    > arangodb create tls keyfile --host localhost --host 127.0.0.1
    > arangodb create client-auth ca
    > arangodb create client-auth keyfile
    # JWT Secret
    > echo 'abc' > /tmp/secret
	# Create cluster A
    > cd /tmp/a
    > arangodb --server.arangod=/usr/sbin/arangod \
         --starter.local=true --starter.port=9528 \
         --auth.jwt-secret=/tmp/secret --starter.address=localhost \
         --starter.sync=true --server.storage-engine=rocksdb \
         --sync.master.jwt-secret=/tmp/secret \
         --sync.server.keyfile=/tmp/certs/tls.keyfile \
         --sync.server.client-cafile=/tmp/certs/client-auth-ca.crt \
         --server.arangosync=/usr/sbin/arangosync&
	# Create cluster B
	> cd /tmp/b
	> arangodb --server.arangod=/usr/sbin/arangod \
         --starter.local=true --starter.port=10528 \
         --auth.jwt-secret=/tmp/secret --starter.address=localhost \
         --starter.sync=true --server.storage-engine=rocksdb \
         --sync.master.jwt-secret=/tmp/secret \
         --sync.server.keyfile=/tmp/certs/tls.keyfile \
         --sync.server.client-cafile=/tmp/certs/client-auth-ca.crt \
         --server.arangosync=/usr/sbin/arangosync&
	# Wait for the dust to settle and start syncing
	> arangosync configure sync \ 
		--master.endpoint=https://localhost:10532 \
		--master.cacert=/tmp/certs/tls-ca.crt \ 
		--master.keyfile=/tmp/certs/client-auth.keyfile \ 
		--source.endpoint=https://localhost:9532 \
		--source.cacert=/tmp/certs/tls-ca.crt --auth.user=root
	# Cluster B is now slave to cluster A it is in read-only mode
	# Check what's going on in background
	> arangosync get tasks --master.endpoint=https://localhost:10532 --master.cacert=/tmp/certs/tls-ca.crt --auth.user=root
	# Stop syncing (Will take some time to ensure consistency)
	> arangosync stop sync --master.endpoint=https://localhost:10532 --master.cacert=/tmp/certs/tls-ca.crt --auth.user=root
	```
* **Kubernetes** operator
	* [GKE](https://github.com/arangodb/kube-arangodb/blob/master/docs/Manual/Tutorials/Kubernetes/GKE.md) 
	* [EKS](https://github.com/arangodb/kube-arangodb/blob/master/docs/Manual/Tutorials/Kubernetes/EKS.md)
	* [Bare metal](https://github.com/arangodb/kube-arangodb/blob/master/docs/Manual/Tutorials/Kubernetes/bare-metal.md)
* **Rolling upgrades** on a kubernetes cluster : Update image string to newer version
	```YAML
	apiVersion: "database.arangodb.com/v1alpha"
	kind: "ArangoDeployment"
	metadata:
	  name: "example-simple-cluster"
	spec:
	  mode: Cluster
	  image: arangodb/arangodb:3.4.2
	  ```
* Troubleshooting
	* [Lost agent, lost coordinator, lost db server](https://docs.arangodb.com/devel/Manual/Administration/Starter/Recovery.html)
	```bash
	# Stop service on one machine
	c13 > sudo systemctl stop arangodb 
	# Delete all files to mimick full disaster
	c13 > rm -rf /opt/arangodb/*
	# Enact disaster recovery
	c13 > echo '192.168.10.13:9000' > /opt/arangodb/RECOVERY
	c13 > sudo systemctl start arangodb
	c13 > tail -f /var/log/syslog
	```
	```bash
	... |INFO| Trying to recover as starter 192.168.10.13:9000 component=arangodb
    ... |INFO| Recovery information all available, starting... component=arangodb
    ... |INFO| ArangoDB Starter listening on 0.0.0.0:9000 (:9000) component=arangodb
    ... |INFO| Found ID from localhost peer component=arangodb
    ... |INFO| Found ID 'd9d45af3' from peer, looking for '87ede24a' component=arangodb
    ... |INFO| Contacting master http://c11:9000... component=arangodb
    ... |INFO| Waiting for 3 servers to show up... component=arangodb
    ... |INFO| Serving as slave with ID '87ede24a' on :9000... component=arangodb
    ...
    ... |INFO| Removed RECOVERY file. component=arangodb
    ... |INFO| Most likely there is now an extra coordinator & dbserver in FAILED state. Remove them manually using the web UI. component=arangodb
   ```
	
	* Rebalancing of shards

## Discussion
