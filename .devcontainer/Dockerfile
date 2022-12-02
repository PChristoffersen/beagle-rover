# See here for image contents: https://github.com/microsoft/vscode-dev-containers/tree/v0.245.2/containers/ubuntu/.devcontainer/base.Dockerfile

# [Choice] Ubuntu version (use ubuntu-22.04 or ubuntu-18.04 on local arm64/Apple Silicon): ubuntu-22.04, ubuntu-20.04, ubuntu-18.04
ARG VARIANT="ubuntu-20.04"
FROM mcr.microsoft.com/vscode/devcontainers/base:0-${VARIANT}

# [Optional] Uncomment this section to install additional OS packages.
# RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
#     && apt-get -y install --no-install-recommends <your-package-list-here>

RUN dpkg --add-architecture i386 \
    && apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install --no-install-recommends \
        libc6:i386 \
        build-essential \
        cmake \
        gcc-8-arm-linux-gnueabihf \
        g++-8-arm-linux-gnueabihf \
        python3-venv \
        python3-pip

# Download beaglebone rootfs snapshot from OneDrive
RUN mkdir /opt/beaglebone && cd /opt/beaglebone \
    && curl -L curl -L 'https://api.onedrive.com/v1.0/shares/s!AgDwcNBDtd2ImYRA_8upN4f_KF3NJQ/root/content' | xz -d | tar x -C /opt/beaglebone \
    && chown root:root am33xx-debian-10.3-iot-armhf

# Install PRU compiler and support package
ARG PRU_CGT_VERSION=2.3.3
RUN cd /opt/beaglebone \
    && curl "https://dr-download.ti.com/software-development/ide-configuration-compiler-or-debugger/MD-FaNNGkDH7s/${PRU_CGT_VERSION}/ti_cgt_pru_${PRU_CGT_VERSION}_linux_installer_x86.bin" --output /tmp/ti_cgt_pru_linux_installer_x86.bin \
    && chmod a+x /tmp/ti_cgt_pru_linux_installer_x86.bin \
    && /tmp/ti_cgt_pru_linux_installer_x86.bin --mode unattended --prefix /opt/beaglebone \
    && rm /tmp/ti_cgt_pru_linux_installer_x86.bin

ARG PRU_SUPPORT_VERSION=6.1.0
RUN cd /opt/beaglebone \
    && curl "https://git.ti.com/cgit/pru-software-support-package/pru-software-support-package/snapshot/pru-software-support-package-${PRU_SUPPORT_VERSION}.tar.xz" | xz -d | tar x

ENV PRU_CGT=/opt/beaglebone/ti-cgt-pru_${PRU_CGT_VERSION}
ENV PRU_SUPPORT=/opt/beaglebone/pru-software-support-package-${PRU_SUPPORT_VERSION}


COPY cmake-tools-kits.json /home/vscode/.local/share/CMakeTools/
COPY am33xx-toolchain.cmake /opt/beaglebone/
