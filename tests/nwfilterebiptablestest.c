/*
 * nwfilterebiptablestest.c: Test {eb,ip,ip6}tables rule generation
 *
 * Copyright (C) 2014 Red Hat, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 */

#include <config.h>

#include "testutils.h"
#include "nwfilter/nwfilter_ebiptables_driver.h"
#include "virbuffer.h"

#define __VIR_FIREWALL_PRIV_H_ALLOW__
#include "virfirewallpriv.h"

#define __VIR_COMMAND_PRIV_H_ALLOW__
#include "vircommandpriv.h"

#define VIR_FROM_THIS VIR_FROM_NONE

static int
testNWFilterEBIPTablesAllTeardown(const void *opaque ATTRIBUTE_UNUSED)
{
    virBuffer buf = VIR_BUFFER_INITIALIZER;
    const char *expected =
        "iptables -D libvirt-out -m physdev --physdev-is-bridged --physdev-out vnet0 -g FO-vnet0\n"
        "iptables -D libvirt-out -m physdev --physdev-out vnet0 -g FO-vnet0\n"
        "iptables -D libvirt-in -m physdev --physdev-in vnet0 -g FI-vnet0\n"
        "iptables -D libvirt-host-in -m physdev --physdev-in vnet0 -g HI-vnet0\n"
        "iptables -D libvirt-in-post -m physdev --physdev-in vnet0 -j ACCEPT\n"
        "iptables -F FO-vnet0\n"
        "iptables -X FO-vnet0\n"
        "iptables -F FI-vnet0\n"
        "iptables -X FI-vnet0\n"
        "iptables -F HI-vnet0\n"
        "iptables -X HI-vnet0\n"
        "ip6tables -D libvirt-out -m physdev --physdev-is-bridged --physdev-out vnet0 -g FO-vnet0\n"
        "ip6tables -D libvirt-out -m physdev --physdev-out vnet0 -g FO-vnet0\n"
        "ip6tables -D libvirt-in -m physdev --physdev-in vnet0 -g FI-vnet0\n"
        "ip6tables -D libvirt-host-in -m physdev --physdev-in vnet0 -g HI-vnet0\n"
        "ip6tables -D libvirt-in-post -m physdev --physdev-in vnet0 -j ACCEPT\n"
        "ip6tables -F FO-vnet0\n"
        "ip6tables -X FO-vnet0\n"
        "ip6tables -F FI-vnet0\n"
        "ip6tables -X FI-vnet0\n"
        "ip6tables -F HI-vnet0\n"
        "ip6tables -X HI-vnet0\n"
        "ebtables -t nat -D PREROUTING -i vnet0 -j libvirt-I-vnet0\n"
        "ebtables -t nat -D POSTROUTING -o vnet0 -j libvirt-O-vnet0\n"
        "ebtables -t nat -L libvirt-I-vnet0\n"
        "ebtables -t nat -L libvirt-O-vnet0\n"
        "ebtables -t nat -F libvirt-I-vnet0\n"
        "ebtables -t nat -X libvirt-I-vnet0\n"
        "ebtables -t nat -F libvirt-O-vnet0\n"
        "ebtables -t nat -X libvirt-O-vnet0\n";
    char *actual = NULL;
    int ret = -1;

    virCommandSetDryRun(&buf, NULL, NULL);

    if (ebiptables_driver.allTeardown("vnet0") < 0)
        goto cleanup;

    if (virBufferError(&buf))
        goto cleanup;

    actual = virBufferContentAndReset(&buf);
    virtTestClearCommandPath(actual);

    if (STRNEQ_NULLABLE(actual, expected)) {
        virtTestDifference(stderr, actual, expected);
        goto cleanup;
    }

    ret = 0;
 cleanup:
    virCommandSetDryRun(NULL, NULL, NULL);
    virBufferFreeAndReset(&buf);
    VIR_FREE(actual);
    return ret;
}


static int
testNWFilterEBIPTablesTearOldRules(const void *opaque ATTRIBUTE_UNUSED)
{
    virBuffer buf = VIR_BUFFER_INITIALIZER;
    const char *expected =
        "iptables -D libvirt-out -m physdev --physdev-is-bridged --physdev-out vnet0 -g FO-vnet0\n"
        "iptables -D libvirt-out -m physdev --physdev-out vnet0 -g FO-vnet0\n"
        "iptables -D libvirt-in -m physdev --physdev-in vnet0 -g FI-vnet0\n"
        "iptables -D libvirt-host-in -m physdev --physdev-in vnet0 -g HI-vnet0\n"
        "iptables -F FO-vnet0\n"
        "iptables -X FO-vnet0\n"
        "iptables -F FI-vnet0\n"
        "iptables -X FI-vnet0\n"
        "iptables -F HI-vnet0\n"
        "iptables -X HI-vnet0\n"
        "iptables -E FP-vnet0 FO-vnet0\n"
        "iptables -E FJ-vnet0 FI-vnet0\n"
        "iptables -E HJ-vnet0 HI-vnet0\n"
        "ip6tables -D libvirt-out -m physdev --physdev-is-bridged --physdev-out vnet0 -g FO-vnet0\n"
        "ip6tables -D libvirt-out -m physdev --physdev-out vnet0 -g FO-vnet0\n"
        "ip6tables -D libvirt-in -m physdev --physdev-in vnet0 -g FI-vnet0\n"
        "ip6tables -D libvirt-host-in -m physdev --physdev-in vnet0 -g HI-vnet0\n"
        "ip6tables -F FO-vnet0\n"
        "ip6tables -X FO-vnet0\n"
        "ip6tables -F FI-vnet0\n"
        "ip6tables -X FI-vnet0\n"
        "ip6tables -F HI-vnet0\n"
        "ip6tables -X HI-vnet0\n"
        "ip6tables -E FP-vnet0 FO-vnet0\n"
        "ip6tables -E FJ-vnet0 FI-vnet0\n"
        "ip6tables -E HJ-vnet0 HI-vnet0\n"
        "ebtables -t nat -D PREROUTING -i vnet0 -j libvirt-I-vnet0\n"
        "ebtables -t nat -D POSTROUTING -o vnet0 -j libvirt-O-vnet0\n"
        "ebtables -t nat -L libvirt-I-vnet0\n"
        "ebtables -t nat -L libvirt-O-vnet0\n"
        "ebtables -t nat -F libvirt-I-vnet0\n"
        "ebtables -t nat -X libvirt-I-vnet0\n"
        "ebtables -t nat -F libvirt-O-vnet0\n"
        "ebtables -t nat -X libvirt-O-vnet0\n"
        "ebtables -t nat -L libvirt-J-vnet0\n"
        "ebtables -t nat -L libvirt-P-vnet0\n"
        "ebtables -t nat -E libvirt-J-vnet0 libvirt-I-vnet0\n"
        "ebtables -t nat -E libvirt-P-vnet0 libvirt-O-vnet0\n";
    char *actual = NULL;
    int ret = -1;

    virCommandSetDryRun(&buf, NULL, NULL);

    if (ebiptables_driver.tearOldRules("vnet0") < 0)
        goto cleanup;

    if (virBufferError(&buf))
        goto cleanup;

    actual = virBufferContentAndReset(&buf);
    virtTestClearCommandPath(actual);

    if (STRNEQ_NULLABLE(actual, expected)) {
        virtTestDifference(stderr, actual, expected);
        goto cleanup;
    }

    ret = 0;
 cleanup:
    virCommandSetDryRun(NULL, NULL, NULL);
    virBufferFreeAndReset(&buf);
    VIR_FREE(actual);
    return ret;
}


static int
testNWFilterEBIPTablesRemoveBasicRules(const void *opaque ATTRIBUTE_UNUSED)
{
    virBuffer buf = VIR_BUFFER_INITIALIZER;
    const char *expected =
        "ebtables -t nat -D PREROUTING -i vnet0 -j libvirt-I-vnet0\n"
        "ebtables -t nat -D POSTROUTING -o vnet0 -j libvirt-O-vnet0\n"
        "ebtables -t nat -L libvirt-I-vnet0\n"
        "ebtables -t nat -L libvirt-O-vnet0\n"
        "ebtables -t nat -F libvirt-I-vnet0\n"
        "ebtables -t nat -X libvirt-I-vnet0\n"
        "ebtables -t nat -F libvirt-O-vnet0\n"
        "ebtables -t nat -X libvirt-O-vnet0\n"
        "ebtables -t nat -D PREROUTING -i vnet0 -j libvirt-J-vnet0\n"
        "ebtables -t nat -D POSTROUTING -o vnet0 -j libvirt-P-vnet0\n"
        "ebtables -t nat -L libvirt-J-vnet0\n"
        "ebtables -t nat -L libvirt-P-vnet0\n"
        "ebtables -t nat -F libvirt-J-vnet0\n"
        "ebtables -t nat -X libvirt-J-vnet0\n"
        "ebtables -t nat -F libvirt-P-vnet0\n"
        "ebtables -t nat -X libvirt-P-vnet0\n";
    char *actual = NULL;
    int ret = -1;

    virCommandSetDryRun(&buf, NULL, NULL);

    if (ebiptables_driver.removeBasicRules("vnet0") < 0)
        goto cleanup;

    if (virBufferError(&buf))
        goto cleanup;

    actual = virBufferContentAndReset(&buf);
    virtTestClearCommandPath(actual);

    if (STRNEQ_NULLABLE(actual, expected)) {
        virtTestDifference(stderr, actual, expected);
        goto cleanup;
    }

    ret = 0;
 cleanup:
    virCommandSetDryRun(NULL, NULL, NULL);
    virBufferFreeAndReset(&buf);
    VIR_FREE(actual);
    return ret;
}


static int
mymain(void)
{
    int ret = 0;

    if (virFirewallSetBackend(VIR_FIREWALL_BACKEND_DIRECT) < 0) {
        ret = -1;
        goto cleanup;
    }

    if (virtTestRun("ebiptablesAllTeardown",
                    testNWFilterEBIPTablesAllTeardown,
                    NULL) < 0)
        ret = -1;

    if (virtTestRun("ebiptablesTearOldRules",
                    testNWFilterEBIPTablesTearOldRules,
                    NULL) < 0)
        ret = -1;

    if (virtTestRun("ebiptablesRemoveBasicRules",
                    testNWFilterEBIPTablesRemoveBasicRules,
                    NULL) < 0)
        ret = -1;

 cleanup:
    return ret == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}

VIRT_TEST_MAIN(mymain)
