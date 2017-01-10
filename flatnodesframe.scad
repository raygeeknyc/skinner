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
o = 30;
p = 65;
r2 = 17;
q = 6;
b = 4.5;
x = 57;
y = 84;
z = 30;
g = 20;
union() {
    /* This makes the bottom off center holder for a Teensy 3.2 with a lip, a channel for imageprocessor cables and part of a channel for panel to Teensy cables.
    */
    translate([2*h+t2+2*t1, w+2*t2-z-(y+2*t1),0]) {
        difference() {
            cube([x+t1, y+2*t1, n+t1+t2]);
            translate([0, t1, t1]) cube([x, y, n+t2]);
            translate([0, y+t1-b/2, t1]) {
                rotate([90, 0, 90]) cylinder(h=t1, d=b);
            }
            translate([x-b, 0, t1]) {
                rotate([0, 90, 90]) cylinder(h=2*t1, d=b);
            }
        }
    }
   /* This makes the top off center holder for a Raspberry Pi 0 with a lip and a slot for the ribbon cable and a channel for GPIO & power cables.
    */
    translate([-1*(o+t1),((w+2*t2)/4)-(p+2*t1)/2,0]) {
        difference() {
            cube([o+t1, p+2*t1, n+t1+t2]);
            translate([t1, t1, t1]) cube([o, p, n+t2]);
            translate([t1+q, t1+p, t1]) cube([r2, t1, t2]);
            translate([(o+t1)/2, 0, t1]) {
                rotate([0, 90, 90]) cylinder(h=t1*2, d=b);
            }
        }
    }
    difference() {
        /* This makes 2 hook shaped holders at the edges of the frame top.
        */
    union() {
        translate([-1*(t1+t2/2), 0, 0]) cube([t1+t2/2, 2*t1+2*t2,t1]);
        translate([-1*(t1+t2), t1+t2, 0]) cylinder(d=2*t1+2*t2, t1);
      }
      translate([-1*(t1+t2), t1+t2, 0]) cylinder(d=2*t1, t1);
      translate([-1*(t2+t1), t1+t2, 0]) cube([t1,t1+t2,t1]);
    }
    difference() {
    union() {
        translate([-1*(t1+t2/2), w-2*t1, 0]) cube([t1+t2/2, 2*t1+2*t2,t1]);
        translate([-1*(t1+t2), w-t1+t2, 0]) cylinder(d=2*t1+2*t2, t1);
      }
      translate([-1*(t1+t2), w-t1+t2, 0]) cylinder(d=2*t1, t1);
      translate([-1*(t2+t1), w-t1+t2, 0]) cube([t1,t1+t2,t1]);
    }
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
        translate([h-g, t2+z-t1, 0]) cube([s, w-2*(z-t1), t1]);
    
        translate([t1+h+t2, t2, t1]) cube([h, w, n]);
        translate([t1+h+t2+m, t2, t1]) cube([h-2*m, w, n+t2]);
        translate([t1+h+g,  t2+z-t1, 0]) cube([s, w-2*(z-t1), t1]);
        
        /* This adds part of the channel for the panels to Teensy cables.
        */
        translate([2*h+2*t1+t2, w+2*t2-z-(y+2*t1),0]) {
            translate([-2*t1, y+t1-b/2, t1]) {
                rotate([90, 0, 90]) cylinder(h=2*t1, d=b);
            }
        }
    }
}