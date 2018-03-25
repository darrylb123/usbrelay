# Builds the usbrelay binary using docker. Docker needs to be installed for this to work.

IMAGE_NAME='usbrelay'
IMAGE_TAG='v1'

# Build the docker image.
sudo docker build -t $IMAGE_NAME:$IMAGE_TAG .

# build the usbrelay source inside the container.
sudo docker run --rm -v "$PWD":/build -w /build $IMAGE_NAME:$IMAGE_TAG make

# Give file ownership back to the host user:group.
sudo chown $USER:$USER usbrelay
