#! /usr/bin/bash

rcnode

webots &
disown

cd ~/robocomp/components/webots-bridge/
pwd

gnome-terminal --tab --execute bash -c "bin/Webots2Robocomp etc/config ; bash"

cd ~/robocomp/components/robocomp-robolab/components/hardware/laser/lidar3D/
pwd

gnome-terminal --tab --execute bash -c "bin/Lidar3D etc/config_helios_webots ; bash"

cd ~/robocomp/components/robocomp-robolab/components/hardware/external_control/joystickpublish/
pwd

gnome-terminal --tab --execute bash -c "bin/JoystickPublish etc/config_shadow ; bash"

cd ~

git clone https://github.com/FranMUex/Proyecto-Robotica-25-26

cd ~/Proyecto-Robotica-25-26/
