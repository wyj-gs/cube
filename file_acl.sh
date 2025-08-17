#!/bin/sh
pkill envset_proc
rm -f instance/server/aspect_policy.cfg
rm -f instance/user/aspect_policy.cfg
yes|cp instance/server/plugin_config.cfg.access instance/server/plugin_config.cfg
yes|cp instance/server/sys_config.cfg.label instance/server/sys_config.cfg
yes|cp instance/server/router_policy.cfg.user instance/server/router_policy.cfg
yes|cp instance/server/aspect_policy.cfg.access instance/server/aspect_policy.cfg

yes|cp instance/user/sys_config.cfg.user instance/user/sys_config.cfg
yes|cp instance/user/router_policy.cfg.user instance/user/router_policy.cfg
sleep 1

sh run_cube.sh exec_def/_user_server.def &
sleep 1
yes | rm instance/user/*.txt
sh run_cube.sh exec_def/_user_client.def login.msg file1.msg file2.msg file3.msg  ctrl_exit.msg
sleep 1
yes | rm instance/user/*.txt
sh run_cube.sh exec_def/_user_client.def login1.msg file1.msg file2.msg file3.msg  ctrl_exit.msg
sleep 1
yes | rm instance/user/*.txt
sh run_cube.sh exec_def/_user_client.def login2.msg file1.msg file2.msg file3.msg  ctrl_exit.msg
