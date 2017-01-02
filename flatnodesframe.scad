// CSG.scad - Basic example of CSG usage

n = 3;
h = 80;
w = 320;
m = 2;
t1 = 3;
t2 = 2;
s = 5;
d = 12;
c = 9;
r = 6;
union() {
    /* This makes the centered holder for a camera with a lip and a slot for the ribbon cable at the bottom.
    */
    translate([-1*(d+t2+t1),(w+2*t2)/2,0]) {
        difference() {
            cube([d+t2+t1, c+2*t1, n+t1]);
            translate([d+t1, ((c+2*t1)-r)/2, 0]) cube([t2, r, t1]);
            translate([t1, t1, t1]) cube([d+t1, c, n]);
        }
    }
    /* This makes the frame that holds 2 panels with slight lips on the topmost and bottommost horizontal edges and slots for cables out the back.
    */
    difference() {
        cube([2*h+t2+2*t1, w+2*t2, n+t1+t2]);
    
        translate([t1, t2, t1]) cube([h, w, n]);
        translate([t1+m, t2, t1]) cube([h-t2-m, w, n+t2]);
        translate([(h-s)/2, t1, 0]) cube([s, w-2*t1, t1]);
    
        translate([t1+h+t2, t2, t1]) cube([h, w, n]);
        translate([t1+h+t2, t2, t1]) cube([h-m, w, n+t2]);
        translate([h+(h-s)/2, t1, 0]) cube([s, w-2*t1, t1]);
    }
}