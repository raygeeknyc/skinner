include <defs.scad>

/* This makes the bottom off center holder for a Teensy 3.2 with a lip, a channel for imageprocessor cables and part of a channel for panel to Teensy cables.
*/
union() {
    difference() {
        cube([x+t1, y+2*t1, n+t1+t2]);
        translate([0, t1, t1]) cube([x, y, n+t2]);
        translate([x-b, 0, t1]) {
            rotate([0, 90, 90]) cylinder(h=2*t1, d=b);
        }
    }
    /* This makes the top off center holder for a Raspberry Pi 0 with a lip and a slot for the ribbon cable and a channel for GPIO & power cables.
    */
    difference() {
        cube([o+t1, p+2*t1, n+t1]);
        translate([t1, t1, t1]) cube([o, p, n]);
        translate([t1+q, t1+p, t2]) cube([r2, t1, t1]);
        translate([(o+t1)/2, 0, t1]) {
            rotate([0, 90, 90]) cylinder(h=t1*2, d=b);
        }
    }
}