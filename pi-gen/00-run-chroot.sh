apt update
apt upgrade -y
apt purge nodejs npm
curl -fsSL https://deb.nodesource.com/setup_20.x -o nodesource_setup.sh
bash nodesource_setup.sh
rm nodesource_setup.sh