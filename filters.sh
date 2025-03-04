#!/bin/bash
sudo tc qdisc del dev enp2s0.9 handle ffff: ingress
sudo tc qdisc add dev enp2s0.9 handle ffff: ingress
sudo tc filter add dev enp2s0.9 parent ffff: protocol ip prio 1 flower ip_proto icmp src_ip 192.168.9.9 action mirred egress redirect dev TunApp
