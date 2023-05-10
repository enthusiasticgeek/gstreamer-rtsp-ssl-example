#!/bin/bash
echo -ne "*** Installing libraries/dependencies ***\n"
sudo apt-get update
declare -a PACKAGES=( 'libgstreamer1.0-dev' 'libgstrtspserver-1.0-dev' 'gstreamer1.0-libav' 'gstreamer1.0-libav' 'gstreamer1.0-plugins-bad' 'gstreamer1.0-plugins-ugly' 'libx264-dev' )
for REQUIRED_PKG in ${PACKAGES[@]}
do
	PKG_OK=$(dpkg-query -W --showformat='${Status}\n' $REQUIRED_PKG|grep "install ok installed")
	echo " **** [INFO] Checking for ${REQUIRED_PKG}: ${PKG_OK}"
	if [ "" = "${PKG_OK}" ]; then
	   echo " **** [WARN] No ${REQUIRED_PKG}. Setting up ${REQUIRED_PKG}."
	   sudo apt-get --yes install ${REQUIRED_PKG}
	else
	   echo " **** [INFO] Great! ${REQUIRED_PKG} is present...proceeding."
	fi
done
