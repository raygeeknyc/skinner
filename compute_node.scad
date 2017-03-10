include <defs.scad>

/* This makes the bottom off center holder for a Teensy 3.2 with a lip, a channel for imageprocessor cables and part of a channel for panel to Teensy cables.
*/
    /* This makes the top off center holder for a Raspberry Pi 0 with a lip and a slot for the ribbon cable and a channel for GPIO & power cables.
    */
    difference() {
        cube([3*t1+td+pid+t1,2*t1+ch, n+t1]);
        translate([t1, t1, t1]) cube([td, ch, n]);
        translate([2*t1+td, t1, t1]) cube([pid+t1, ch, n]);
        translate([2*t1+b, 0, t1]) {
            rotate([0, 90, 90]) cylinder(h=t1*2, d=b);
            translate([0, t1, 0]) cylinder(h=t1*2, d=b);
        }
        translate([2*t1+b, ch, t1]) {
            rotate([0, 90, 90]) cylinder(h=t1*2, d=b);
            translate([0, t1, 0]) cylinder(h=t1*2, d=b);
        }
        translate([0, ch-b, t1]) {
            rotate([90, 0, 90]) cylinder(h=t1*2, d=b);
            translate([t1, 0, 0]) cylinder(h=t1*2, d=b);
        }
        translate([t1/2+td, ch-b, t1]) {
            rotate([90, 0, 90]) cylinder(h=t1*2, d=b);
        }
        translate([2*t1+td+pid/2-cp/2+t1, 0, t1/2]) {
            cube([cp, 2*t1, t1]);
        }
    }