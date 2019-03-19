# Builds the usbrelay binary using docker. Docker needs to be installed for this to work.

IMAGE_NAME='usbrelay'
IMAGE_TAG='v2'

# Build the docker image.
sudo docker build -t $IMAGE_NAME:$IMAGE_TAG .

# build the usbrelay source inside the container.
sudo docker run --rm -v "$PWD":/build -w /build $IMAGE_NAME:$IMAGE_TAG make python

# Give file ownership back to the host user:group.
sudo chown -R $USER:$USER usbrelay libusbrelay.so libusbrelay_py.so build/
