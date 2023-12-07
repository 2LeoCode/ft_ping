FROM debian:latest

WORKDIR /root/ft_ping
RUN     apt-get update; \
        apt-get upgrade; \
        apt-get install -y \
          make \
          gcc;