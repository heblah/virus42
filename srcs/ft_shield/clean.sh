#! /bin/bash

sudo systemctl disable kthread
sudo systemctl stop kthread
sudo rm /var/lock/.matt_daemon
sudo rm /etc/systemd/system/kthread.service
sudo rm /usr/local/bin/kthread
