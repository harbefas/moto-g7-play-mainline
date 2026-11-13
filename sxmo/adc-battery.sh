#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-only
# Battery % for sxmo without a PMI632 fuel gauge driver.
# Reads vbat from the PMI632 ADC, maps it through a Li-ion (4.4V) OCV curve.
# Accuracy ~±10%; charging inflates vbat, so subtract an offset then.

adc=$(grep -l pmic@2:adc /sys/bus/iio/devices/iio:device*/name | xargs dirname)

while :; do
	vbat=$(cat "$adc/in_voltage_vbat_sns_input")
	usbv=$(cat "$adc/in_voltage_usb_in_v_div_16_input")

	if [ "$usbv" -gt 4000000 ]; then
		state=charging
		vbat=$((vbat - 30000))
	else
		state=discharging
	fi

	pct=$(echo "$vbat" | awk '{
		n = split("4380 4300 4200 4100 4000 3920 3860 3820 3790 3760 3720 3680 3600 3400", v)
		split("100 95 85 75 63 53 43 33 23 15 8 4 1 0", p)
		mv = $1 / 1000
		if (mv >= v[1]) { print 100; exit }
		for (i = 2; i <= n; i++)
			if (mv >= v[i]) {
				printf "%d\n", p[i] + (mv - v[i]) * (p[i-1] - p[i]) / (v[i-1] - v[i])
				exit
			}
		print 0
	}')

	[ "$state" = charging ] && [ "$pct" -ge 100 ] && state=fully-charged

	sxmo_hook_statusbar.sh battery pmi632 "$state" "$pct"
	sleep 60
done
