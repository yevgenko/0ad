## Required Tools

    sudo apt-get install debootstrap schroot

## Create Isolated Workspace

    sudo mkdir -p /srv/chroot/0ad_bionic_amd64
    sudo debootstrap --variant=buildd --arch=amd64 bionic /srv/chroot/0ad_bionic_amd64 http://archive.ubuntu.com/ubuntu/
    cd path/to/0ad/source
    sudo cp -R ./.devchroot/conf_example/etc/schroot /etc
    sudo schroot -c 0ad_bionic_amd64 -u root -p
    apt-get update
    exit

To use schroot service without **sudo** prefix and without password add `root-users=pete` property into `/etc/schroot/chroot.d/0ad_bionic_amd64.conf` where **pete** is your current user name.

See the difference between **users** and **root-users** in the `man schroot.conf`

## Fix Locale

    sudo schroot -c 0ad_bionic_amd64 -u root -p
    apt-get install language-pack-en
    locale-gen en_GB.UTF-8
    exit

## Build Project

    cd path/to/0ad/source
    sudo schroot -c 0ad_bionic_amd64 -u root -p -- cat .devchroot/requirements.txt | xargs apt-get install -y
    sudo schroot -c 0ad_bionic_amd64 -u $USERNAME -p
    cd build/workspaces
    ./update-workspaces.sh -j7
    cd gcc
    make -j7
    exit

## Running Tests

    cd path/to/0ad/source
    sudo schroot -c 0ad_bionic_amd64 -u $USERNAME -p -- binaries/system/test

## Running Game

    cd path/to/0ad/source
    sudo schroot -c 0ad_bionic_amd64 -u $USERNAME -p -- binaries/system/pyrogenesis

## Troubleshooting

### No sound

speaker-test

ALSA lib pcm_dmix.c:1052:(snd_pcm_dmix_open) unable to open slave
AL lib: (EE) ALCplaybackAlsa_open: Could not open playback device
'default': No such file or directory
ERROR: Sound: AlcInit failed, m_Device=0x0 m_Context=0x0 dev_name=OpenAL
Soft err=a005

Try to add **/etc/modprobe.d/default.conf**:

    options snd_hda_intel index=1

and restart, worked last time, as per
http://forums.debian.net/viewtopic.php?f=6&t=123902