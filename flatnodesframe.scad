include <defs.scad>

union() {
    difference() {
        /* This makes 2 hook shaped holders at the edges of the frame top.
        */
        union() {
            translate([-1*(t1+t2/2), 0, 0]) cube([t1+t2/2, 2*t1+2*t2,t3]);
            translate([-1*(t1+t2), t1+t2, 0]) cylinder(d=2*t1+2*t2, t3);
        }
        translate([-1*(t1+t2), t1+t2, 0]) cylinder(d=2*t1, t3);
        translate([-1*(t2+t1), t1+t2, 0]) cube([t1,t1+t2,t3]);
    }
    difference() {
        union() {
            translate([-1*(t1+t2/2), w-2*t1, 0]) cube([t1+t2/2, 2*t1+2*t2,t3]);
            translate([-1*(t1+t2), w-t1+t2, 0])  cylinder(d=2*t1+2*t2, t3);
        }
        translate([-1*(t1+t2), w-t1+t2, 0]) cylinder(d=2*t1, t3);
        translate([-1*(t2+t1), w-t1+t2, 0]) cube([t1,t1+t2,t3]);
    }
    
    /* This makes the centered holder for a camera with a lip and slots for the ribbon cable at the bottom and the top edge.
    */
    translate([-1*(d+t2+t1),(w+2*t2)/2,0]) {
        difference() {
            cube([d+t2+t1, d+2*t1, n+sch]);
            translate([t1, t1, sch]) cube([d+t1, d, n]); // the cavity
            translate([0, t1+((d-cc)/2), t2]) cube([t1, cc, n+sch-t2-t2/2]);  // the topside slot for the ribbon
        }
    }
 
    /* This makes the frame that holds 2 panels with slight lips on the topmost and bottommost horizontal edges and slots for cables out the back.
    There are 2 channels to be used for braces after the model has been cut into 2 sections for printing.
    */
    difference() {
        cube([2*h+t2+2*t1, w+2*t2, n+t3+t2]);
    
        // This makes a hole for the power and data cable into the top panel.
        translate([0,(w+2*t2)/2-channelo,conn/2+1]) rotate([90, 90, 90]) scale([1.0, 1.5, 1.0]) cylinder(h=t2+h-p2-conn/2, d=conn);
        
        // This makes a channel for the input power and data to the top panel input
        translate([t2+h-p2-conn/2, t2+z+t1, conn/2+1]) {
            rotate([90, 90, 180]) scale([1.0, 1.5, 1.0]) cylinder(h=((w+2*t2)/2-channelo)-(z-t2/2), d=conn);
        translate([0, 0, -1*(conn/2)]) rotate([0, 0, 90]) cylinder(h=20, d=conn*2);
       }
       
    // This makes the overhang in the center divider
    translate([t1, t2, t3]) cube([h, w, n]);    
    translate([t1+m, t2, t3]) cube([h-t2-m, w, n+t2]);
 
     // This makes the channel from the top panel to the bottom one
     translate([t2+h-p2-conn/2, w-z, conn/2+1]) {
        translate([0, 0, -1*(conn/2)]) rotate([0, 0, 90]) cylinder(h=20, d=conn*2);
          rotate([90, 90, 90]) scale([1.0, 1.5, 1.0]) cylinder(h=h, d=conn);
       translate([h, 0, -1*(conn/2)]) rotate([0, 0, 90]) cylinder(h=20, d=conn*2);
        }
        
        // This makes the channel across the bottom panel
       translate([t2+h+h-p2-conn/2, t2+z+t1, conn/2+1]) {
            rotate([90, 90, 180]) scale([1.0, 1.5, 1.0]) cylinder(h=w-z*2, d=conn);
        translate([0, 0, -1*(conn/2)]) rotate([0, 0, 90]) cylinder(h=20, d=conn*2);
       }

        // These make the overhangs at top and bottom
        translate([t1+h+t2, t2, t3]) cube([h, w, n]);    
        translate([t1+h+t2+m, t2, t3]) cube([h-2*m, w, n+t2]);
        // These are the channels for braces
        translate([t1+((h-g)/2)-(l2/2), (w/2)-(l1/2),t2]) cube([l2, l1, t3-t2]);
       translate([t1+h+t2+s/2+g+l2, (w/2)-(l1/2),t2]) cube([l2, l1, t3-t2]);
    }
}