FROM debian
RUN apt update && apt -y install ganglia-monitor
RUN gmond -f -c /etc/ganglia/gmond.conf
