# Copyright 1999-2005 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: /cvsroot/Join_projects/wlan_bt_linx_pci/application/3dsp-wifi-radar/ubuntu/3dsp-wifi-radar-0.3.0-ubuntu8.10/wifi-radar.ebuild,v 1.1 2009/06/12 02:28:30 spear.wang Exp $

inherit eutils

DESCRIPTION="WiFi Radar is a Python/PyGTK2  utility for managing WiFi profiles."
HOMEPAGE="http://www.bitbuilder.com/wifi_radar/"
SRC_URI="http://www.bitbuilder.com/wifi_radar/bins/${PN}-${PV}.tar.gz"

LICENSE="GPL-2"
SLOT="0"
KEYWORDS="~x86"
IUSE=""

DEPEND=">=dev-python/pygtk-2
	net-wireless/wireless-tools"

src_install() {
	dosbin wifi-radar
	dobin  wifi-radar.sh
	doicon wifi-radar.svg
	doicon wifi-radar.png
	dodoc  README
	dodoc  ChangeLog
	dodoc  AUTHORS
	dodoc  INSTALL
	make_desktop_entry wifi-radar.sh "WiFi Radar"
	insinto /etc/conf.d ; newins wifi-radar.conf wifi-radar.conf
}