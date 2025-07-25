DESCRIPTION = "Watchpoint kernel module"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=751419260aa954499f7abaabaa882bbe"

SRC_URI = "file://watchpoint.c \
           file://Makefile \
           file://COPYING"

S = "${WORKDIR}"

inherit module

EXTRA_OEMAKE = "KERNELDIR=${STAGING_KERNEL_DIR}"

do_compile() {
    oe_runmake
}

do_install() {
    install -d ${D}${nonarch_base_libdir}/modules/${KERNEL_VERSION}/extra
    install -m 0644 watchpoint.ko ${D}${nonarch_base_libdir}/modules/${KERNEL_VERSION}/extra/
}
