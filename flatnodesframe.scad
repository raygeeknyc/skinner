// CSG.scad - Basic example of CSG usage

n = 3;
h = 80;
w = 320;
m = 2;
t1 = 3;
t2 = 2;
s = 5;

/* This makes the frame that holds 2 panels with slight lips on
   the topmost and bottommost horizontal edges and slots for cables out the back.
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
