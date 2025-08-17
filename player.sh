#!/bin/sh
pkill envset_proc
pkill main_proc

rm -f instance/server/aspect_policy.cfg
rm -f instance/user/aspect_policy.cfg
yes|cp instance/server/plugin_config.cfg.access instance/server/plugin_config.cfg
yes|cp instance/server/sys_config.cfg.label instance/server/sys_config.cfg
yes|cp instance/server/router_policy.cfg.record instance/server/router_policy.cfg
yes|cp instance/server/aspect_policy.cfg.access instance/server/aspect_policy.cfg

yes|cp instance/user/sys_config.cfg.record instance/user/sys_config.cfg
yes|cp instance/user/router_policy.cfg.record instance/user/router_policy.cfg
yes|cp instance/user/aspect_policy.cfg.flag instance/user/aspect_policy.cfg
sleep 1

export CUBEAPPPATH=`pwd` 
export CUBE_APP_PLUGIN=$CUBEAPPPATH/plugin/
export CUBE_DEFINE_PATH=$CUBEAPPPATH/define/:$CUBE_DEFINE_PATH
cd instance/player
/root/centoscloud/cube-1.3/proc/main/main_proc trigger_start.msg start.msg
cd -
