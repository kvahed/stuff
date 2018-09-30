#!/bin/bash
prometheus_version="2.4.2"

started=$(pwd)

echo "install arangodb exporters"
cd ~/
echo "  download and install exporters service"
wget https://github.com/arangodb-helper/arangodb-exporter/releases/download/0.1.3/arangodb-exporter-linux-amd64
sudo mv arangodb-exporter-linux-amd64 /usr/local/bin/arangodb-exporter
sudo chmod 755 /usr/local/bin/arangodb-exporter

echo "  install systemd services for exporters"
cd /etc/systemd/system
for i in coordinator dbserver agent; do
    sudo wget https://raw.githubusercontent.com/kvahed/stuff/master/jackie/arango-${i}-exporter.service
done
sudo systemctl daemon-reload
sudo systemctl enable arango-agent-exporter
sudo systemctl enable arango-dbserver-exporter
sudo systemctl enable arango-coordinator-exporter
sudo systemctl start arango-agent-exporter
sudo systemctl start arango-dbserver-exporter
sudo systemctl start arango-coordinator-exporter
echo

echo "install and configure prometheus"
cd ~
echo "  setup up prometheus user/group"
sudo groupadd --system prometheus
sudo useradd -s /sbin/nologin --system -g prometheus prometheus

echo "  download and install prometheus ${prometheus_version}"

wget https://github.com/prometheus/prometheus/releases/download/v${prometheus_version}/prometheus-${prometheus_version}.linux-amd64.tar.gz
tar xzf prometheus-${prometheus_version}.linux-amd64.tar.gz
cd prometheus-${prometheus_version}.linux-amd64
sudo cp prometheus /usr/local/bin
sudo cp promtool /usr/local/bin
sudo mkdir -p /etc/prometheus
sudo cp -r consoles/ console_libraries/ /etc/prometheus/
sudo mkdir -p /etc/prometheus/rules
sudo mkdir -p /etc/prometheus/rules.d
sudo mkdir -p /etc/prometheus/files_sd
sudo mkdir -p /var/lib/prometheus
cd ..
sudo rm -rf prometheus-${prometheus_version}*
for i in rules rules.d files_sd; do
    sudo chown -R prometheus:prometheus /etc/prometheus/${i};
done
for i in rules rules.d files_sd; do
    sudo chmod -R 775 /etc/prometheus/${i};
done
sudo chown -R prometheus:prometheus /var/lib/prometheus/

echo "  configure prometheus"
cd /etc/prometheus
wget https://raw.githubusercontent.com/kvahed/stuff/master/jackie/prometheus.yml

echo "  install / enable / start prometheus service"
cd /etc/systemd/system
sudo wget https://raw.githubusercontent.com/kvahed/stuff/master/jackie/prometheus.service
sudo systemctl daemon-reload
sudo systemctl start prometheus

cd ${started}
echo
