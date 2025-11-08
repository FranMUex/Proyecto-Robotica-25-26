#! /usr/bin/bash

bash ~/robocomp/tools/rcnode/rcnode.sh &

cd /usr/local/bin

if ! test -f robocomp; then
  sudo ln -s /home/usuario/software/pip_env/bin/robocomp
  sudo ln -s /home/usuario/software/pip_env/bin/robocompdsl
fi

webots &
disown

sleep 3

cd ~/robocomp/components/webots-bridge/

gnome-terminal --tab --execute bash -c "bin/Webots2Robocomp etc/config ; bash"

cd ~/robocomp/components/robocomp-robolab/components/hardware/laser/lidar3D/

gnome-terminal --tab --execute bash -c "bin/Lidar3D etc/config_pearl_webots ; bash"

cd ~/robocomp/components/robocomp-robolab/components/hardware/external_control/joystickpublish/

gnome-terminal --tab --execute bash -c "bin/JoystickPublish etc/config_shadow ; bash"
