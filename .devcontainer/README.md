## Requirements

* VSCode with Remote-Containers extension

## Build

In VSCode, open ~/path/to/0ad folder in container, after the build complete
open terminal in vscode and proceed to compiling, see next section.

### Compiling the code

    cd build/workspaces
    ./update-workspaces.sh -j7
    cd gcc
    make -j7

### Testing

    cd ../../..
    binaries/system/test

## Running 0ad via docker

    sudo docker run --rm -v $PWD:/workspaces/0ad -w /workspaces/0ad -e DISPLAY=$DISPLAY -v /tmp/.X11-unix:/tmp/.X11-unix:rw --ipc=host --user $(id -u):$(id -g) --cap-drop=ALL --security-opt=no-new-privileges --group-add video --device /dev/dri vsc-0ad-92d5fd697c2da4589307175a452f880d-uid:latest ./binaries/system/pyrogenesis

Where **vsc-0ad-92d5fd697c2da4589307175a452f880d-uid** is the image name build from Dockerfile in current directory
and **$PWD** is pointing to project's root directory.

### Troubleshooting

If there problems with permissions to access X11 you might try the following command to resolve:

    xhost +SI:localuser:$(id -un)

See original guide on running X11 in docker to learn more:

https://github.com/mviereck/x11docker/wiki/Short-setups-to-provide-X-display-to-container

With hardware acceleration:
https://github.com/mviereck/x11docker/wiki/Hardware-acceleration

Sound (not tested):
https://github.com/mviereck/x11docker/wiki/Container-sound:-ALSA-or-Pulseaudio