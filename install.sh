#!/bin/sh

set -e

INSTALL_DIR=/opt/beaglerover

PYTHON_MODULE=beaglerover.cpython-37m-arm-linux-gnueabihf.so

if [ ! -d "$INSTALL_DIR" ]; then
    echo "Install dir $INSTALL_DIR does not exist"
    exit 1
fi

if [ ! -f build/$PYTHON_MODULE ]; then
    echo "Python module not build $PYTHON_MODULE"
    exit 1
fi

if [ ! -f web/src/robotweb/www/index.html ]; then
    echo "Web client not built"
    exit 1
fi


echo "Copying python module $PYTHON_MODULE"
install -m 755 --strip build/$PYTHON_MODULE $INSTALL_DIR/

#echo "Stripping $PYTHON_MODULE"
#strip $INSTALL_DIR/$PYTHON_MODULE

echo "Copying web server"
rsync -a --delete --exclude __pycache__ --exclude .gitignore web/src/robotweb $INSTALL_DIR/
install -m 775 web/src/main.py $INSTALL_DIR/

echo "Installing startup script"
cat > $INSTALL_DIR/beaglerover.service << EOF
[Unit]
Description=BeagleRover Web UI
After=syslog.target network.target

[Service]
Type=simple
User=root
WorkingDirectory=/opt/beaglerover
ExecStart=$INSTALL_DIR/main.py --port 80
#Restart=on-abort
Restart=no
# Wait 30 seconds before the unit starts.
ExecStartPre=/bin/sleep 20

[Install]
WantedBy=multi-user.target
EOF

echo "Enabling startup script"
sudo ln -sfv $INSTALL_DIR/beaglerover.service --target-directory=/etc/systemd/system/
sudo systemctl daemon-reload

