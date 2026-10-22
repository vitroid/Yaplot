#!/usr/bin/env python3
"""Generate a natural 3D growing & blooming tree as a Yaplot startup demo.

Features:
- Monopodial trunk with sympodial crown & golden-angle phyllotaxis along nodes
- Natural tropism: arched boughs with upward-curving tips
- Tapered wood: sticks (`s`) for trunk/boughs, lines (`l`) for fine twigs
- Buds and leaves (`c`) that swell smoothly from radius 0 as shoots extend
- Blossoms that open across the crown and scatter petals in the late breeze
- Layers: 1 = wood, 2 = foliage, 3 = blossoms (toggle with F1/F2/F3 in Yaplot)
- SVG overlay: number of branches, leaves and petals vs. time on a log scale

Regenerate: python3 utils/gen_koch_demo.py -o help.yap
"""

from __future__ import annotations

import argparse
import math
import random
from dataclasses import dataclass
from pathlib import Path

Vec3 = tuple[float, float, float]

_PHI = (1.0 + math.sqrt(5.0)) / 2.0
GOLDEN_ANGLE = 2.0 * math.pi * (1.0 - 1.0 / _PHI)  # ~137.508 deg


def add(a: Vec3, b: Vec3) -> Vec3:
    return (a[0] + b[0], a[1] + b[1], a[2] + b[2])


def scale(v: Vec3, s: float) -> Vec3:
    return (v[0] * s, v[1] * s, v[2] * s)


def lerp(a: Vec3, b: Vec3, t: float) -> Vec3:
    return (
        a[0] + (b[0] - a[0]) * t,
        a[1] + (b[1] - a[1]) * t,
        a[2] + (b[2] - a[2]) * t,
    )


def norm(v: Vec3) -> float:
    return math.sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2])


def normalize(v: Vec3) -> Vec3:
    n = norm(v) or 1.0
    return (v[0] / n, v[1] / n, v[2] / n)


def cross(a: Vec3, b: Vec3) -> Vec3:
    return (
        a[1] * b[2] - a[2] * b[1],
        a[2] * b[0] - a[0] * b[2],
        a[0] * b[1] - a[1] * b[0],
    )


def orthonormal_basis(direction: Vec3) -> tuple[Vec3, Vec3]:
    d = normalize(direction)
    helper = (0.0, 1.0, 0.0) if abs(d[1]) < 0.9 else (1.0, 0.0, 0.0)
    x = normalize(cross(helper, d))
    y = cross(d, x)
    return x, y


def smoothstep(t: float) -> float:
    if t <= 0.0:
        return 0.0
    if t >= 1.0:
        return 1.0
    return t * t * (3.0 - 2.0 * t)


@dataclass
class Segment:
    p0: Vec3
    p1: Vec3
    radius: float  # mature radius after secondary thickening
    order: int
    t0: float
    t1: float
    is_root: bool = False


@dataclass
class ApicalBud:
    """Leaves riding the extending shoot tip (apical meristem) while it grows."""

    p0: Vec3
    p1: Vec3
    offset: Vec3
    t0: float
    t1: float
    max_r: float
    palette: int


@dataclass
class LeafCluster:
    pos: Vec3
    t0: float
    max_r: float
    palette: int  # 5 or 6
    fade_t0: float = 9.9  # primary stem leaves senesce once inner boughs thicken


@dataclass
class Blossom:
    pos: Vec3
    normal: Vec3
    t0: float
    fall_t0: float  # >1.0 if never falls
    drift: Vec3
    phase: float
    rosette: bool  # True = 5-petal rosette, False = single bud/blossom


def build_natural_tree(
    rng: random.Random,
) -> tuple[list[Segment], list[ApicalBud], list[LeafCluster], list[Blossom]]:
    segments: list[Segment] = []
    apical_buds: list[ApicalBud] = []
    leaves: list[LeafCluster] = []
    blossoms: list[Blossom] = []

    spiral_counter = 0

    def next_azimuth(jitter: float = 0.10) -> float:
        nonlocal spiral_counter
        spiral_counter += 1
        return spiral_counter * GOLDEN_ANGLE + rng.uniform(-jitter, jitter)

    def grow_shoot(
        p0: Vec3,
        direction: Vec3,
        length: float,
        radius: float,
        order: int,
        t0: float,
        internodes: int,
        phi0: float,
    ) -> None:
        cur_p = p0
        cur_dir = normalize(direction)
        cur_r = radius
        cur_len = length
        cur_t = t0
        local_phi = phi0

        for step in range(internodes):
            dt = 0.032 + 0.006 * order
            t1 = cur_t + dt

            # Organic curvature + tropism:
            # Boughs arch outward first, then tips curve gently toward the sky
            x_ax, y_ax = orthonormal_basis(cur_dir)
            wander_ang = rng.uniform(0.0, 2.0 * math.pi)
            wander = add(
                scale(x_ax, math.cos(wander_ang)),
                scale(y_ax, math.sin(wander_ang)),
            )
            if order == 0:
                sway = (
                    0.05 * math.sin(step * 1.5),
                    0.05 * math.cos(step * 1.5),
                    0.24,
                )
                seg_dir = normalize(add(cur_dir, sway))
            elif order == 1:
                up_pull = -0.04 if step == 0 else (0.09 + 0.06 * step)
                seg_dir = normalize(
                    add(add(cur_dir, scale(wander, 0.07)), (0.0, 0.0, up_pull))
                )
            elif order == 2:
                up_pull = 0.06 + 0.05 * step
                seg_dir = normalize(
                    add(add(cur_dir, scale(wander, 0.11)), (0.0, 0.0, up_pull))
                )
            else:
                seg_dir = normalize(
                    add(add(cur_dir, scale(wander, 0.13)), (0.0, 0.0, 0.08))
                )

            p1 = add(cur_p, scale(seg_dir, cur_len))

            segments.append(
                Segment(
                    p0=cur_p,
                    p1=p1,
                    radius=cur_r,
                    order=order,
                    t0=cur_t,
                    t1=t1,
                )
            )

            is_last_step = step == internodes - 1
            bx, by = orthonormal_basis(seg_dir)

            # 1) Apical leaves riding the growing tip of EVERY shoot from the very start
            for k in range(2):
                ang = next_azimuth()
                off = add(
                    scale(bx, math.cos(ang) * 0.34),
                    scale(by, math.sin(ang) * 0.34),
                )
                apical_buds.append(
                    ApicalBud(
                        p0=cur_p,
                        p1=p1,
                        offset=add(off, (0.0, 0.0, 0.14)),
                        t0=cur_t,
                        t1=t1,
                        max_r=0.38 if order <= 1 else 0.44,
                        palette=6 if k == 0 else 5,
                    )
                )

            # 2) Young nodal leaves left behind on trunk & primary boughs as the shoot passes;
            #    they later senesce (fade_t0) once the upper canopy shades the interior.
            if order <= 1:
                n_young = 2 if (order == 0 and step == 0) else 1
                for k in range(n_young):
                    ang = next_azimuth()
                    off = add(
                        scale(bx, math.cos(ang) * 0.44),
                        scale(by, math.sin(ang) * 0.44),
                    )
                    lpos = add(p1, add(off, (0.0, 0.0, 0.12)))
                    leaves.append(
                        LeafCluster(
                            pos=lpos,
                            t0=t1 - 0.008,
                            max_r=rng.uniform(0.38, 0.48),
                            palette=6 if (step + k) % 2 == 0 else 5,
                            fade_t0=t1 + (0.20 if order == 0 else 0.26),
                        )
                    )

            # 3) Permanent crown foliage on secondary and tertiary shoots
            if order >= 2:
                n_leaf = 1 if (order == 2 and not is_last_step) else 2
                for k in range(n_leaf):
                    ang = next_azimuth()
                    off = add(
                        scale(bx, math.cos(ang) * 0.52),
                        scale(by, math.sin(ang) * 0.52),
                    )
                    lpos = add(p1, add(off, (0.0, 0.0, rng.uniform(-0.12, 0.28))))
                    pal = 5 if (k + order + step) % 2 == 0 else 6
                    leaves.append(
                        LeafCluster(
                            pos=lpos,
                            t0=t1 - 0.005,
                            max_r=rng.uniform(0.48, 0.68),
                            palette=pal,
                        )
                    )

            # Blossoms on outer twigs
            if order >= 3 or (order == 2 and is_last_step):
                if rng.random() < (0.80 if is_last_step else 0.45):
                    bx, by = orthonormal_basis(seg_dir)
                    ang = next_azimuth()
                    off = add(
                        scale(bx, math.cos(ang) * 0.36),
                        scale(by, math.sin(ang) * 0.36),
                    )
                    bpos = add(p1, add(off, (0.0, 0.0, 0.24)))
                    bloom_t0 = max(t1 + 0.05, 0.48 + rng.uniform(0.0, 0.14))
                    falls = rng.random() < 0.45
                    fall_t0 = rng.uniform(0.74, 0.90) if falls else 9.9
                    drift = (
                        rng.uniform(-4.4, 4.4),
                        rng.uniform(-4.4, 4.4),
                        rng.uniform(-12.0, -8.4),
                    )
                    blossoms.append(
                        Blossom(
                            pos=bpos,
                            normal=seg_dir,
                            t0=bloom_t0,
                            fall_t0=fall_t0,
                            drift=drift,
                            phase=rng.uniform(0.0, 2.0 * math.pi),
                            rosette=is_last_step and (rng.random() < 0.45),
                        )
                    )

            # Lateral branching at internodes via golden angle around THIS shoot
            if order < 3:
                can_branch = (order > 0) or (step >= 1)
                if can_branch:
                    n_lat = 2 if (order == 0 and step >= 2) else 1
                    bx, by = orthonormal_basis(seg_dir)
                    for b_i in range(n_lat):
                        local_phi += GOLDEN_ANGLE + rng.uniform(-0.08, 0.08)
                        az = local_phi
                        pitch_deg = (
                            rng.uniform(48.0, 62.0)
                            if order == 0
                            else rng.uniform(32.0, 44.0)
                        )
                        pitch = math.radians(pitch_deg)
                        side = add(scale(bx, math.cos(az)), scale(by, math.sin(az)))
                        lat_dir = normalize(
                            add(
                                scale(seg_dir, math.cos(pitch)),
                                scale(side, math.sin(pitch)),
                            )
                        )
                        lat_r = cur_r * (0.65 if order == 0 else 0.68)
                        lat_len = cur_len * rng.uniform(0.82, 0.92)
                        lat_nodes = 4 if order == 0 else 3
                        grow_shoot(
                            p1,
                            lat_dir,
                            lat_len,
                            lat_r,
                            order + 1,
                            t1 + 0.005 * b_i,
                            lat_nodes,
                            local_phi + 0.7,
                        )

            # Crown apex: trunk divides into 3 sympodial leaders
            if order == 0 and is_last_step:
                bx, by = orthonormal_basis(seg_dir)
                for k in range(3):
                    local_phi += GOLDEN_ANGLE
                    az = local_phi
                    pitch = math.radians(rng.uniform(34.0, 44.0))
                    side = add(scale(bx, math.cos(az)), scale(by, math.sin(az)))
                    lead_dir = normalize(
                        add(
                            scale(seg_dir, math.cos(pitch)),
                            scale(side, math.sin(pitch)),
                        )
                    )
                    grow_shoot(
                        p1,
                        lead_dir,
                        cur_len * 0.88,
                        cur_r * 0.68,
                        1,
                        t1 + 0.004 * k,
                        4,
                        az,
                    )

            cur_p = p1
            cur_dir = seg_dir
            cur_r *= 0.82 if order == 0 else 0.78
            cur_len *= 0.92 if order == 0 else 0.88
            cur_t = t1

    # Root flare at ground z = -11.0 (develops gradually as the trunk thickens)
    base_z = -11.0
    trunk_base = (0.0, 0.0, base_z)
    for k in range(5):
        ang = k * GOLDEN_ANGLE
        foot = (2.30 * math.cos(ang), 2.30 * math.sin(ang), base_z - 0.70)
        collar = (0.40 * math.cos(ang), 0.40 * math.sin(ang), base_z + 1.10)
        segments.append(
            Segment(
                p0=collar,
                p1=foot,
                radius=0.44,
                order=0,
                t0=0.08,
                t1=0.22,
                is_root=True,
            )
        )

    grow_shoot(
        p0=trunk_base,
        direction=(0.0, 0.0, 1.0),
        length=2.90,
        radius=0.72,
        order=0,
        t0=0.0,
        internodes=5,
        phi0=0.0,
    )

    return segments, apical_buds, leaves, blossoms


def quant_r(r: float) -> float:
    return max(0.08, round(r * 12.5) / 12.5)


def emit_frame(
    segments: list[Segment],
    apical_buds: list[ApicalBud],
    leaves: list[LeafCluster],
    blossoms: list[Blossom],
    t: float,
) -> tuple[list[str], dict[str, int]]:
    """Return the frame's commands and the number of branches, leaves and petals drawn."""

    def xf(p: Vec3) -> Vec3:
        return p

    lines: list[str] = []

    # --- Layer 1: Ground meadow (subdivided into small tiles for clean depth sorting) + Wood ---
    lines.append("y 1")
    lines.append("@ 9")
    ground_z = -11.75
    radii = [0.0, 1.8, 3.8, 6.4, 9.5]
    n_sectors = 16
    d_ang = 2.0 * math.pi / n_sectors
    for ir in range(len(radii) - 1):
        r0, r1 = radii[ir], radii[ir + 1]
        for k in range(n_sectors):
            a0 = k * d_ang
            a1 = (k + 1) * d_ang
            if r0 == 0.0:
                p_c = xf((0.0, 0.0, ground_z))
                p_1 = xf((r1 * math.cos(a0), r1 * math.sin(a0), ground_z))
                p_2 = xf((r1 * math.cos(a1), r1 * math.sin(a1), ground_z))
                lines.append(
                    f"p 3 {p_c[0]:.3f} {p_c[1]:.3f} {p_c[2]:.3f} "
                    f"{p_1[0]:.3f} {p_1[1]:.3f} {p_1[2]:.3f} "
                    f"{p_2[0]:.3f} {p_2[1]:.3f} {p_2[2]:.3f}"
                )
            else:
                q0 = xf((r0 * math.cos(a0), r0 * math.sin(a0), ground_z))
                q1 = xf((r1 * math.cos(a0), r1 * math.sin(a0), ground_z))
                q2 = xf((r1 * math.cos(a1), r1 * math.sin(a1), ground_z))
                q3 = xf((r0 * math.cos(a1), r0 * math.sin(a1), ground_z))
                lines.append(
                    f"p 4 {q0[0]:.3f} {q0[1]:.3f} {q0[2]:.3f} "
                    f"{q1[0]:.3f} {q1[1]:.3f} {q1[2]:.3f} "
                    f"{q2[0]:.3f} {q2[1]:.3f} {q2[2]:.3f} "
                    f"{q3[0]:.3f} {q3[1]:.3f} {q3[2]:.3f}"
                )

    thick_buckets: dict[tuple[int, float], list[str]] = {}
    fine_twigs: list[str] = []

    for seg in segments:
        if t <= seg.t0:
            continue
        # Primary growth: shoot elongation from p0 to p1
        g_len = smoothstep((t - seg.t0) / max(seg.t1 - seg.t0, 1e-6))
        p0 = xf(seg.p0)
        p1 = xf(lerp(seg.p0, seg.p1, g_len))

        # Secondary growth (cambial thickening):
        # Every shoot starts as a slender green/young stem (r_tip ~ 0.08) and thickens
        # gradually over time into woody bark as the canopy above it expands.
        r_tip = 0.08
        thicken_span = 0.36 if seg.order == 0 else (0.30 if seg.order == 1 else 0.22)
        g_thick = smoothstep((t - seg.t0) / thicken_span)
        r_raw = r_tip + (seg.radius - r_tip) * g_thick

        if seg.order <= 2 and seg.radius >= 0.14 and r_raw >= 0.11:
            # Young slender shoots look like lighter branch wood (pal 4) before maturing into dark bark (pal 3)
            pal = 3 if (seg.order <= 1 and g_thick > 0.35) else 4
            r_now = quant_r(r_raw)
            key = (pal, r_now)
            thick_buckets.setdefault(key, []).append(
                f"s {p0[0]:.3f} {p0[1]:.3f} {p0[2]:.3f} "
                f"{p1[0]:.3f} {p1[1]:.3f} {p1[2]:.3f}"
            )
        else:
            fine_twigs.append(
                f"l {p0[0]:.3f} {p0[1]:.3f} {p0[2]:.3f} "
                f"{p1[0]:.3f} {p1[1]:.3f} {p1[2]:.3f}"
            )

    if thick_buckets or fine_twigs:
        cur_pal = -1
        for (pal, r_val), seg_cmds in sorted(thick_buckets.items()):
            if pal != cur_pal:
                lines.append(f"@ {pal}")
                cur_pal = pal
            lines.append(f"r {r_val:.2f}")
            lines.extend(seg_cmds)
        if fine_twigs:
            if cur_pal != 4:
                lines.append("@ 4")
            lines.extend(fine_twigs)

    # --- Layer 2: Foliage (apical tip leaves + nodal & crown leaf clusters) ---
    leaf_buckets: dict[tuple[int, float], list[str]] = {}

    # Apical leaves that ride the tip of every growing shoot
    for ab in apical_buds:
        if t <= ab.t0 or t >= ab.t1 + 0.04:
            continue
        g_pos = smoothstep((t - ab.t0) / max(ab.t1 - ab.t0, 1e-6))
        tip = lerp(ab.p0, ab.p1, g_pos)
        grow_in = smoothstep((t - ab.t0) / 0.020)
        fade_out = 1.0 - smoothstep((t - ab.t1) / 0.040)
        r_now = quant_r(ab.max_r * grow_in * fade_out)
        if r_now < 0.08:
            continue
        p = xf(add(tip, scale(ab.offset, 0.5 + 0.5 * grow_in)))
        leaf_buckets.setdefault((ab.palette, r_now), []).append(
            f"c {p[0]:.3f} {p[1]:.3f} {p[2]:.3f}"
        )

    for lf in leaves:
        if t <= lf.t0:
            continue
        g = smoothstep((t - lf.t0) / 0.09)
        if t > lf.fade_t0:
            fade = 1.0 - smoothstep((t - lf.fade_t0) / 0.12)
            g *= fade
        if g <= 0.12:
            continue
        r_now = quant_r(lf.max_r * g)
        if r_now < 0.06:
            continue
        p = xf(lf.pos)
        leaf_buckets.setdefault((lf.palette, r_now), []).append(
            f"c {p[0]:.3f} {p[1]:.3f} {p[2]:.3f}"
        )

    if leaf_buckets:
        lines.append("y 2")
        cur_pal = -1
        for (pal, r_val), l_cmds in sorted(leaf_buckets.items()):
            if pal != cur_pal:
                lines.append(f"@ {pal}")
                cur_pal = pal
            lines.append(f"r {r_val:.2f}")
            lines.extend(l_cmds)

    # --- Layer 3: Blossoms & falling petals ---
    blossom_buckets: dict[tuple[int, float], list[str]] = {}

    for bl in blossoms:
        if t <= bl.t0:
            continue
        bloom = smoothstep((t - bl.t0) / 0.12)
        if bloom < 0.10:
            continue

        # Falling petal in late breeze
        if t > bl.fall_t0:
            ft = min(1.0, (t - bl.fall_t0) / 0.25)
            sway_x = 1.30 * math.sin(6.0 * ft + bl.phase)
            sway_y = 1.30 * math.cos(5.0 * ft + bl.phase)
            pos_world = (
                bl.pos[0] + bl.drift[0] * ft + sway_x,
                bl.pos[1] + bl.drift[1] * ft + sway_y,
                max(-11.6, bl.pos[2] + bl.drift[2] * ft),
            )
            p = xf(pos_world)
            blossom_buckets.setdefault((7, 0.24), []).append(
                f"c {p[0]:.3f} {p[1]:.3f} {p[2]:.3f}"
            )
            continue

        center = xf(bl.pos)
        if bl.rosette:
            r_core = quant_r(0.22 * bloom)
            blossom_buckets.setdefault((8, r_core), []).append(
                f"c {center[0]:.3f} {center[1]:.3f} {center[2]:.3f}"
            )
            u_vec, v_vec = orthonormal_basis(bl.normal)
            petal_dist = 0.44 * bloom
            r_petal = quant_r(0.28 * bloom)
            for k in range(5):
                ang = bl.phase + k * (2.0 * math.pi / 5.0)
                offset = add(
                    scale(u_vec, math.cos(ang) * petal_dist),
                    scale(v_vec, math.sin(ang) * petal_dist),
                )
                pt = xf(add(bl.pos, offset))
                blossom_buckets.setdefault((7, r_petal), []).append(
                    f"c {pt[0]:.3f} {pt[1]:.3f} {pt[2]:.3f}"
                )
        else:
            r_fl = quant_r(0.40 * bloom)
            blossom_buckets.setdefault((7, r_fl), []).append(
                f"c {center[0]:.3f} {center[1]:.3f} {center[2]:.3f}"
            )

    if blossom_buckets:
        lines.append("y 3")
        cur_pal = -1
        for (pal, r_val), b_cmds in sorted(blossom_buckets.items()):
            if pal != cur_pal:
                lines.append(f"@ {pal}")
                cur_pal = pal
            lines.append(f"r {r_val:.2f}")
            lines.extend(b_cmds)

    counts = {
        "branches": sum(len(c) for c in thick_buckets.values()) + len(fine_twigs),
        "leaves": sum(len(c) for c in leaf_buckets.values()),
        "petals": sum(
            len(c) for (pal, _), c in blossom_buckets.items() if pal == 7
        ),
    }
    return lines, counts


CURVES = [
    ("branches", "rgb(120,80,45)"),
    ("leaves", "rgb(40,140,55)"),
    ("petals", "rgb(225,95,145)"),
]
PANEL = (292.0, 368.0, 212.0, 136.0)  # x, y, width, height in the 512x512 viewBox
SVG_OPEN = (
    '<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 512 512" '
    'font-family="Helvetica, Arial, sans-serif">'
)


class CountPlot:
    """SVG overlay plotting log10(count) of branches, leaves and petals against time.

    The curves and axes go into the global block once; each frame only adds the
    time cursor, the current points and the legend with current counts.
    """

    def __init__(self, history: list[dict[str, int]]):
        self.history = history
        peak = max(max(h.values()) for h in history)
        self.decades = max(1, math.ceil(math.log10(max(peak, 2))))
        x, y, w, h = PANEL
        self.x0, self.x1 = x + 36.0, x + w - 10.0
        self.y0, self.y1 = y + 24.0, y + h - 20.0

    def px(self, i: int) -> float:
        return self.x0 + (self.x1 - self.x0) * i / max(len(self.history) - 1, 1)

    def py(self, n: int) -> float:
        return self.y1 - (self.y1 - self.y0) * math.log10(n) / self.decades

    def static_svg(self) -> list[str]:
        x, y, w, h = PANEL
        out = [
            SVG_OPEN,
            f'<rect x="{x}" y="{y}" width="{w}" height="{h}" rx="6" fill="white" '
            'fill-opacity="0.7" stroke="rgb(90,110,130)" stroke-width="0.8"/>',
        ]
        for k in range(self.decades + 1):
            yy = self.py(10**k)
            out.append(
                f'<line x1="{self.x0:.1f}" y1="{yy:.1f}" x2="{self.x1:.1f}" y2="{yy:.1f}" '
                'stroke="rgb(170,170,170)" stroke-width="0.5"/>'
            )
            out.append(
                f'<text x="{self.x0 - 4:.1f}" y="{yy + 3:.1f}" font-size="9" '
                f'text-anchor="end" fill="rgb(60,60,60)">{10**k}</text>'
            )
        out.append(
            f'<polyline points="{self.x0:.1f},{self.y0:.1f} {self.x0:.1f},{self.y1:.1f} '
            f'{self.x1:.1f},{self.y1:.1f}" fill="none" stroke="rgb(60,60,60)" stroke-width="0.8"/>'
        )
        out.append(
            f'<text x="{(self.x0 + self.x1) / 2:.1f}" y="{y + h - 6:.1f}" font-size="9" '
            'text-anchor="middle" fill="rgb(60,60,60)">time</text>'
        )
        for key, color in CURVES:
            runs: list[list[str]] = [[]]
            for i, counts in enumerate(self.history):
                if counts[key] > 0:
                    runs[-1].append(f"{self.px(i):.1f},{self.py(counts[key]):.1f}")
                elif runs[-1]:
                    runs.append([])
            for run in runs:
                if len(run) >= 2:
                    out.append(
                        f'<polyline points="{" ".join(run)}" fill="none" '
                        f'stroke="{color}" stroke-width="1.5"/>'
                    )
        out.append("</svg>")
        return out

    def frame_svg(self, i: int) -> list[str]:
        x, y, _, _ = PANEL
        cx = self.px(i)
        out = [
            SVG_OPEN,
            f'<line x1="{cx:.1f}" y1="{self.y0:.1f}" x2="{cx:.1f}" y2="{self.y1:.1f}" '
            'stroke="rgb(60,60,60)" stroke-width="0.8" stroke-dasharray="2,2"/>',
        ]
        for j, (key, color) in enumerate(CURVES):
            n = self.history[i][key]
            if n > 0:
                out.append(
                    f'<circle cx="{cx:.1f}" cy="{self.py(n):.1f}" r="2.5" fill="{color}" '
                    'stroke="white" stroke-width="0.8"/>'
                )
            lx = x + (10.0, 84.0, 148.0)[j]
            ly = y + 15.0
            out.append(f'<circle cx="{lx:.1f}" cy="{ly - 3:.1f}" r="3" fill="{color}"/>')
            out.append(
                f'<text x="{lx + 6:.1f}" y="{ly:.1f}" font-size="9.5" '
                f'fill="rgb(40,40,40)">{key} {n}</text>'
            )
        out.append("</svg>")
        return out


def main() -> None:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("-o", "--output", type=Path, default=Path("help.yap"))
    ap.add_argument("--frames", type=int, default=140)
    ap.add_argument("--seed", type=int, default=11)
    args = ap.parse_args()

    rng = random.Random(args.seed)
    segments, apical_buds, leaves, blossoms = build_natural_tree(rng)

    out: list[str] = [
        "# Yaplot startup demo: 3D growing & blooming tree (sticks, lines, circles, polygon)",
        "# Layers: F1 = ground & wood, F2 = foliage, F3 = blossoms & falling petals",
        "# Generated by utils/gen_koch_demo.py — re-run to regenerate.",
        "global",
        "@ 1 185 225 250",
        "@ 3 95 60 35",
        "@ 4 135 95 60",
        "@ 5 55 150 65",
        "@ 6 95 185 75",
        "@ 7 250 185 205",
        "@ 8 255 220 95",
        "@ 9 125 185 95",
        "r 0.30",
        "y 2 foliage",
        'y 3 "blossoms & petals"',
        'y 1 "ground & wood"',
        "a 0",
        "reality 3",
        "splat 1",
        "fov 45",
        "verbose 1",
        "rotation 0.5",
        'message "=== Yaplot 3D Growing Tree Demo ==="',
        'message "  n / p (PageDown/Up) : Next / previous frame (10n / 10p: skip 10 frames)"',
        'message "  Shift+N / Shift+P   : Play forward / backward (10N / 10P: fast 10 fps)"',
        'message "  SPACE / !           : Stop animation+rotation / Stop rotation only"',
        'message "  g / G               : First / last frame"',
        'message "  Mouse drag / Arrows : Rotate view (TAB to reset)"',
        'message "  F1 / F2 / F3        : Toggle Ground+Wood / Foliage / Blossoms"',
        'message "  + / - and d         : Reality (0..4) and depth splat shadow (0..3)"',
        'message "  [ / ] and * / /     : Field of view & zoom"',
        'message "  v (or 0v..3v)       : Set on-screen info verbosity"',
        'message "  q                   : Quit"',
    ]

    frames: list[list[str]] = []
    history: list[dict[str, int]] = []
    for i in range(args.frames):
        u = i / max(args.frames - 1, 1)
        t = 1.02 * (u**0.90)
        lines, counts = emit_frame(segments, apical_buds, leaves, blossoms, t)
        frames.append(lines)
        history.append(counts)

    plot = CountPlot(history)
    out.extend(plot.static_svg())
    out.append("")
    for i, lines in enumerate(frames):
        if i > 0:
            out.append("")
        out.extend(lines)
        out.extend(plot.frame_svg(i))

    args.output.write_text("\n".join(out) + "\n", encoding="utf-8")
    print(
        f"Wrote {args.output} ({args.frames} frames, "
        f"{len(segments)} segments, {len(apical_buds)} apical buds, "
        f"{len(leaves)} leaf clusters, {len(blossoms)} blossoms)",
        file=__import__("sys").stderr,
    )


if __name__ == "__main__":
    main()
