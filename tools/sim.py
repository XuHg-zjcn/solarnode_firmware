#!/usr/bin/env python3
########################################################################
# DCDC控制仿真程序
# Copyright (C) 2025  Xu Ruijun
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
########################################################################
import math
import random
import numpy as np
import matplotlib.pyplot as plt


class PIControll:
    def __init__(self, kp, ki, s0=0):
        self.kp = kp
        self.ki = ki
        self.s = s0

    def update(self, target, curr):
        err = target - curr
        self.s += self.ki*err
        output = self.s + self.kp*err
        if output < 0 or output > 1:
            output = max(0, min(output, 0.6))
            self.s = output - self.kp*err
        return output


class BoostSim:
    def __init__(self, T, L, Co):
        self.Vin = 0
        self.Vout = 0
        self.I_L = 0
        self.T = T
        self.L = L
        self.Co = Co

    def update(self, D):
        t_on = D*self.T
        t_off = (1-D)*self.T
        dI = self.Vin/self.L*t_on
        Ipeak = self.I_L + dI
        dIdt_off = -(self.Vout-self.Vin)/self.L
        if Ipeak/-dIdt_off > t_off:
            q_out = Ipeak*t_off + dIdt_off*t_off**2/2
        else:
            t_qo = Ipeak/-dIdt_off
            q_out = Ipeak*t_qo/2
        self.Vout += q_out/self.Co


pi = PIControll(0.02, 1e-4)
boost = BoostSim(1/125000, 1.5e-5, 1e-5)
boost.Vin = 18
boost.Vout = 19
Vo_rec = []
D_rec = []
for i in range(100000):
    D = pi.update(48, boost.Vout)
    D = int(D*192)/192
    boost.update(D)
    if i < 50000:
        R = 1e5
    else:
        R = 5e2
    R *= random.gauss(1, 0.05)
    boost.Vout -= max(boost.Vout-0.5, 0)/R*boost.T/boost.Co
    Vo_rec.append(boost.Vout)
    D_rec.append(D)

plt.plot(Vo_rec)
plt.twinx()
plt.plot(D_rec)
plt.show()
