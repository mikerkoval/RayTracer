#include "Raytracer.h"
int Raytracer::closestObjectIndex(vector<double> intersections){
//return index of smallest value greater than 0
    int index = -1;
    if(intersections.size() ==0 ){
        return -1;
    }
    else if (intersections.size() == 1){
        if(intersections.at(0) > .0000000000001 ){
            return 0;
        }
        else{
            return -1; 
        }
    }
    else {
        double max = 0;
        for (int i = 0; i < intersections.size(); i++){
            if(max < intersections.at(i)){
                max = intersections.at(i);
            }
        }
        if(max > 0){
            for (int i = 0; i < intersections.size(); i ++){
                if(intersections.at(i) > .000000000001 && intersections.at(i) <= max ){
                    max = intersections.at(i);
                    index = i;
                }
            }
            

            return index;
        }
        else{
            return -1;
        }
    }
}

static Color skyColor(Vect dir) {
    // t=0 at horizon, t=1 at zenith
    double t = dir.getY();  // -1 to 1
    t = (t + 1.0) * 0.5;   // remap to 0-1
    if (t < 0) t = 0;
    if (t > 1) t = 1;
    // horizon: warm light blue, zenith: deep blue
    double r = 0.55 * (1.0 - t) + 0.1 * t;
    double g = 0.75 * (1.0 - t) + 0.3 * t;
    double b = 0.95 * (1.0 - t) + 0.6 * t;
    return Color(r, g, b, 0);
}

Color Raytracer::getColorAt(Vect intersection_position,Vect intersecting_direction, vector<Object*> scene_objects, int index_closest,vector<Source*> light_sources,double  accuracy,double ambientlight, int n){
  
 

    scene_objects.at(index_closest);
    Color winning_object_color = scene_objects.at(index_closest)->getColor(intersection_position);
    Vect winning_object_normal = scene_objects.at(index_closest)->getNormalAt(intersection_position);
    if (winning_object_color.getSpecial() == 2) {
        // checkered/tile floor pattern
        int square = (int)floor(intersection_position.getX()) + (int)floor(intersection_position.getZ());
        bool cond = ((int)floor(intersection_position.getX() * 10)) %10 == 0 || ((int)floor(intersection_position.getZ() * 10)) %10 == 0 ;
        if (cond ) {
            // dark grid line
            winning_object_color.setRed(0.08);
            winning_object_color.setGreen(0.08);
            winning_object_color.setBlue(0.12);
            winning_object_color.setSpecularity(0.3);
        }

        else {
            // light tile — medium gray so reflections show through
            winning_object_color.setRed(0.38);
            winning_object_color.setGreen(0.38);
            winning_object_color.setBlue(0.45);
            winning_object_color.setSpecularity(0.5);
        }
    }
    if (winning_object_color.getSpecial() == 3) {
        // checkered/tile floor pattern
        
        int square = intersection_position.getX() - intersection_position.getY() + 100;
        bool cond = (int)(square * 10) % 4 == 0 ;
        if (cond ) {
            // black tile
            winning_object_color.setRed(0);
            winning_object_color.setGreen(0);
            winning_object_color.setBlue(0);
        }
        else {
            // white tile
            winning_object_color.setRed(1);
            winning_object_color.setGreen(1);
            winning_object_color.setBlue(1);
        }
    }
    Color final_color = winning_object_color.scalar(ambientlight);
    if (winning_object_color.getSpecularity() > 0 && winning_object_color.getSpecularity() <= 1) {
        // reflection from objects with specular intensity
        double dot1 = winning_object_normal.dotProduct(intersecting_direction.negative());
        Vect scalar1 = winning_object_normal.mult(dot1);
        Vect add1 = scalar1.add(intersecting_direction);
        Vect scalar2 = add1.mult(2);
        Vect add2 = intersecting_direction.negative().add(scalar2);
        Vect reflection_direction = add2.normalize();

        // offset origin along normal to avoid self-intersection
        Vect reflection_ray_origin = intersection_position.add(winning_object_normal.mult(0.01));
        Ray reflection_ray (reflection_ray_origin, reflection_direction);

        // determine what the ray intersects with first
        vector<double> reflection_intersections;

        for (int reflection_index = 0; reflection_index < scene_objects.size(); reflection_index++) {
            if (reflection_index == index_closest && scene_objects.at(reflection_index)->isConvex()) {
                reflection_intersections.push_back(-1);
            } else {
                reflection_intersections.push_back(scene_objects.at(reflection_index)->findIntersection(reflection_ray));
            }
        }

        int index_of_winning_object_with_reflection = closestObjectIndex(reflection_intersections);

        if (index_of_winning_object_with_reflection == -1) {
            final_color = final_color.add(skyColor(reflection_direction).scalar(winning_object_color.getSpecularity()));
        } else if (index_of_winning_object_with_reflection != -1) {
            // reflection ray missed everything else
            if (reflection_intersections.at(index_of_winning_object_with_reflection) > 0.01) {
                // determine the position and direction at the point of intersection with the reflection ray
                // the ray only affects the color if it reflected off something
                
                Vect reflection_intersection_position = intersection_position.add(reflection_direction.mult(reflection_intersections.at(index_of_winning_object_with_reflection)));
                Vect reflection_intersection_ray_direction = reflection_direction;
                if(n > 0){
                    if(reflection_intersection_ray_direction.magnitude()  > 1.1){
                        //cout <<   reflection_intersection_ray_direction.magnitude() << endl;
                    } 
                    Color reflection_intersection_color = getColorAt(reflection_intersection_position, reflection_intersection_ray_direction, scene_objects, index_of_winning_object_with_reflection, light_sources, accuracy, ambientlight, n-1);
                    //if Fresnel 
                    if(winning_object_color.getSpecularity() ==1 && winning_object_color.getTransparency() == 1){
                        double n1 = 1; double n2 = 1.3;
                        Vect n = winning_object_normal;
                        if(winning_object_normal.dotProduct(intersecting_direction) > 0){
                            n1 = 1.3; n2 = 1;
                            n = n.negative();
                        }
                        
                       // cout << "direction " << intersecting_direction.magnitude() << endl;
                        double dot =  n.dotProduct(intersecting_direction);
                        if(dot < 0){ dot = - dot;}

                        double t1 =  acos(dot);
                        if(t1 > 3.14159/2){
                            t1 = t1 - 3.14159/2;
                        }
                        if(n1/n2 * sin(t1) > 1){
                            final_color = final_color.add(reflection_intersection_color.scalar(1));
                        }
                        else{
                            double t2 = asin(n1/n2 * sin(t1));

                            double f1 = tan(t1 - t2)/ tan(t1 + t2);
                            f1 = f1*f1; 
                            double f2 = sin(t1 - t2)/ sin(t1 + t2);
                            f2 = f2*f2; 
                            double fr = 0.5 * (f1 + f2);

                            
                            if(fr != fr){
                                cout << "FR: " <<  fr << endl;
                                cout << "t1: " <<  t1 << endl;
                                cout << "t2: " <<  t2 << endl;
                                cout << "f1: " <<  f1 << endl;
                                cout << "f2: " <<  f2 << endl;
                            }
                            
                            
                            
                            
                            final_color = final_color.add(reflection_intersection_color.scalar(fr));
                        }
                        
                       

                    }
                    // else normal
                    else {
                         final_color = final_color.add(reflection_intersection_color.scalar(winning_object_color.getSpecularity()));

                    }

                }

                
            }
        }
      

     
         
    }
    if (winning_object_color.getTransparency() > 0 && winning_object_color.getTransparency() <= 1) {
        // refraction from objects with specular intensity
       
        double ind = 1/1.3;
        if(winning_object_normal.dotProduct(intersecting_direction) > 0){
            ind = 1.3/1;
        }
        double c1 = winning_object_normal.dotProduct(intersecting_direction);
        if(c1 < 0){ c1 = -c1;}
        double c2 = sqrt(1-(ind*ind) * (1- c1*c1));


        Vect reflection_direction = intersecting_direction.add(winning_object_normal.mult(c1)).mult(ind).add(winning_object_normal.mult(c2).negative());
        reflection_direction = reflection_direction.normalize();
        // offset origin along refraction direction to avoid self-intersection
        Vect refraction_ray_origin = intersection_position.add(reflection_direction.mult(0.001));
        Ray reflection_ray (refraction_ray_origin, reflection_direction);
        
        // determine what the ray intersects with first
        vector<double> reflection_intersections;
        
        for (int reflection_index = 0; reflection_index < scene_objects.size(); reflection_index++) {
            reflection_intersections.push_back(scene_objects.at(reflection_index)->findIntersection(reflection_ray));
        }
        
        int index_of_winning_object_with_reflection = closestObjectIndex(reflection_intersections);
        
        if (index_of_winning_object_with_reflection != -1) {
            // reflection ray missed everthing else
            if (reflection_intersections.at(index_of_winning_object_with_reflection) > 0.01) {
                // determine the position and direction at the point of intersection with the reflection ray
                // the ray only affects the color if it reflected off something
                
                Vect reflection_intersection_position = intersection_position.add(reflection_direction.mult(reflection_intersections.at(index_of_winning_object_with_reflection)));
                Vect reflection_intersection_ray_direction = reflection_direction;
                if(n > 0){
                    
                    Color reflection_intersection_color = getColorAt(reflection_intersection_position, reflection_intersection_ray_direction, scene_objects, index_of_winning_object_with_reflection, light_sources, accuracy, ambientlight, n-1);
                       
                    if(winning_object_color.getSpecularity() ==1 && winning_object_color.getTransparency() == 1){
                        double n1 = 1; double n2 = 1.3;
                        Vect n = winning_object_normal;
                        if(winning_object_normal.dotProduct(intersecting_direction) > 0){
                            n1 = 1.3; n2 = 1;
                            n = n.negative();
                        }
                        
                       // cout << "direction " << intersecting_direction.magnitude() << endl;
                        double dot =  n.dotProduct(intersecting_direction);
                        if(dot < 0){ dot = - dot;}

                        double t1 =  acos(dot);
                        if(t1 > 3.14159/2){
                            t1 = t1 - 3.14159/2;
                        }
                        if(n1/n2 * sin(t1) > 1 ){
                            final_color = final_color.add(reflection_intersection_color.scalar(0));
                        }
                        else{
                            double t2 = asin(n1/n2 * sin(t1));

                            double f1 = tan(t1 - t2)/ tan(t1 + t2);
                            f1 = f1*f1; 
                            double f2 = sin(t1 - t2)/ sin(t1 + t2);
                            f2 = f2*f2; 
                            double fr = 0.5 * (f1 + f2);

                            final_color = final_color.add(reflection_intersection_color.scalar(1-fr));
                        }
                        
                       

                    }
                    // else normal
                    else {
                         final_color = final_color.add(reflection_intersection_color.scalar(winning_object_color.getSpecularity()));

                    }

                }

                
            }
        }
      

     
         
    }
    for (int light_index = 0; light_index < light_sources.size(); light_index++){
        Vect light_direction = light_sources.at(light_index) -> getPosition().add(intersection_position.negative()).normalize();
        float cosine_angle = winning_object_normal.dotProduct(light_direction);
        if(cosine_angle >0 || scene_objects.at(index_closest) ->getCL()){
            //test for shadows 
            
            bool shadowed = false;

            Vect distance_to_light = light_sources.at(light_index)->getPosition().add(intersection_position.negative());
            double distance_to_light_magnitude = distance_to_light.magnitude();

            Vect shadow_origin = intersection_position.add(winning_object_normal.mult(0.05));
            Ray shadow_ray (shadow_origin, light_sources.at(light_index)->getPosition().add(intersection_position.negative()).normalize());

            for (int object_index = 0; object_index < scene_objects.size(); object_index++) {
                if (scene_objects.at(object_index)->getCL()) continue;
                double t = scene_objects.at(object_index)->findIntersection(shadow_ray);
                if (t > 0.01 && t <= distance_to_light_magnitude) {
                    shadowed = true;
                    break;
                }
            }

            if (shadowed == false) {
                if (scene_objects.at(index_closest) ->getCL() ) {
                    final_color = final_color.add(winning_object_color.multiply(light_sources.at(light_index)->getColor()).scalar(1));
                }
                else{
                    final_color = final_color.add(winning_object_color.multiply(light_sources.at(light_index)->getColor()).scalar(cosine_angle));
                }



                // Specular highlight handled by reflection ray above, not Phong term

            }
        }   
    }
    return final_color.clip();
}

static void renderPixelRange(int xstart, int xend, int height, int width, int aadepth,
    float aspectratio, double accuracy, double ambientlight,
    Vect camdir, Vect camright, Vect camdown, Camera scene_cam,
    vector<Object*>& scene_objects, vector<Source*>& light_sources,
    RGBType* pixels)
{
    double xamnt, yamnt;

    for (int x = xstart; x < xend; x++) {
        for (int y = 0; y < height; y++) {
            int thisone = y*width + x;

            int aa_samples = aadepth * aadepth;
            vector<double> tempRed(aa_samples);
            vector<double> tempGreen(aa_samples);
            vector<double> tempBlue(aa_samples);

            for (int aax = 0; aax < aadepth; aax++) {
                for (int aay = 0; aay < aadepth; aay++) {

                    int aa_index = aay*aadepth + aax;

                    // create the ray from the camera to this pixel
                    if (aadepth == 1) {
                        if (width > height) {
                            xamnt = ((x+0.5)/width)*aspectratio - (((width-height)/(double)height)/2);
                            yamnt = ((height - y) + 0.5)/height;
                        }
                        else if (height > width) {
                            xamnt = (x + 0.5)/ width;
                            yamnt = (((height - y) + 0.5)/height)/aspectratio - (((height - width)/(double)width)/2);
                        }
                        else {
                            xamnt = (x + 0.5)/width;
                            yamnt = ((height - y) + 0.5)/height;
                        }
                    }
                    else {
                        // anti-aliasing
                        if (width > height) {
                            xamnt = ((x + (double)aax/((double)aadepth - 1))/width)*aspectratio - (((width-height)/(double)height)/2);
                            yamnt = ((height - y) + (double)aax/((double)aadepth - 1))/height;
                        }
                        else if (height > width) {
                            xamnt = (x + (double)aax/((double)aadepth - 1))/ width;
                            yamnt = (((height - y) + (double)aax/((double)aadepth - 1))/height)/aspectratio - (((height - width)/(double)width)/2);
                        }
                        else {
                            xamnt = (x + (double)aax/((double)aadepth - 1))/width;
                            yamnt = ((height - y) + (double)aax/((double)aadepth - 1))/height;
                        }
                    }

                    Vect cam_ray_origin = scene_cam.getPosition();
                    Vect cam_ray_direction = camdir.add(camright.mult(xamnt - 0.5).add(camdown.mult(yamnt - 0.5))).normalize();

                    Ray cam_ray (cam_ray_origin, cam_ray_direction);

                    vector<double> intersections;
                    for (int index = 0; index < (int)scene_objects.size(); index++) {
                        intersections.push_back(scene_objects.at(index)->findIntersection(cam_ray));
                    }

                    int index_of_winning_object = Raytracer::closestObjectIndex(intersections);

                    if (index_of_winning_object == -1) {
                        Color sky = skyColor(cam_ray_direction);
                        tempRed[aa_index]   = sky.getRed();
                        tempGreen[aa_index] = sky.getGreen();
                        tempBlue[aa_index]  = sky.getBlue();
                    }
                    else {
                        if (intersections.at(index_of_winning_object) > accuracy) {
                            Vect intersection_position = cam_ray_origin.add(cam_ray_direction.mult(intersections.at(index_of_winning_object)));
                            Vect intersecting_ray_direction = cam_ray_direction;

                            Color intersection_color = Raytracer::getColorAt(intersection_position, intersecting_ray_direction, scene_objects, index_of_winning_object, light_sources, accuracy, ambientlight, 5);

                            tempRed[aa_index]   = intersection_color.getRed();
                            tempGreen[aa_index] = intersection_color.getGreen();
                            tempBlue[aa_index]  = intersection_color.getBlue();
                        }
                    }
                }
            }

            // average the pixel color
            double totalRed = 0, totalGreen = 0, totalBlue = 0;
            for (int i = 0; i < aa_samples; i++) {
                totalRed   += tempRed[i];
                totalGreen += tempGreen[i];
                totalBlue  += tempBlue[i];
            }

            pixels[thisone].r = totalRed   / aa_samples;
            pixels[thisone].g = totalGreen / aa_samples;
            pixels[thisone].b = totalBlue  / aa_samples;
        }
    }
}


    
int Raytracer::generate (vector<Object*> objs, vector<Source*>lights, std::string filename, int aa, Vect cp, Vect cd, bool fast){
  
    

    std::cout << "Launched from the main\n";


    campos = cp;
    look_at = cd;
    diff_btw = Vect(campos.getX() - look_at.getX(), campos.getY() - look_at.getY(), campos.getZ() - look_at.getZ());
    
    camdir = diff_btw.negative().normalize();
    camright = Y.crossProduct(camdir).normalize();
    camdown = camright.crossProduct(camdir);
    scene_cam = Camera(campos, camdir, camright, camdown);

    cout << "GENERATING \n";
    aadepth = aa;
    RGBType *pixels = new RGBType[n]();

    
    vector<Source*> light_sources;
    vector <Object*> scene_objects;
    light_sources = lights;
    scene_objects = objs;
   


    int num_threads = fast ? (int)std::thread::hardware_concurrency() : 1;
    if (num_threads < 1) num_threads = 1;
    cout << "Threads: " << num_threads << endl;

    vector<std::thread> threads;
    for (int i = 0; i < num_threads; i++) {
        int xstart = (width * i)       / num_threads;
        int xend   = (width * (i + 1)) / num_threads;
        threads.emplace_back(renderPixelRange,
            xstart, xend,
            height, width, aadepth,
            aspectratio, accuracy, ambientlight,
            camdir, camright, camdown, scene_cam,
            ref(scene_objects), ref(light_sources),
            pixels);
    }
    for (auto& t : threads) {
        t.join();
    }
    cout << "done" << endl;
    savepng(filename.c_str(), width, height, dpi, pixels, n);

    delete pixels;
    auto t2 = std::chrono::steady_clock::now();
    double diff = std::chrono::duration<double>(t2 - t1).count();
    cout << diff << " seconds " << endl;
    return 0; 
}
void Raytracer::savepng (const char *filename, int w, int h, int dpi, RGBType *data, int size){
    cout << "SAVING \n";
    // Build raw 8-bit RGB buffer, flipping rows (renderer stores bottom-to-top)
    vector<unsigned char> buf(w * h * 3);
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int src = (h - 1 - y) * w + x;
            int dst = y * w + x;
            double r = data[src].r < 0 ? 0 : (data[src].r > 1 ? 1 : data[src].r);
            double g = data[src].g < 0 ? 0 : (data[src].g > 1 ? 1 : data[src].g);
            double b = data[src].b < 0 ? 0 : (data[src].b > 1 ? 1 : data[src].b);
            buf[dst*3 + 0] = (unsigned char)(r * 255);
            buf[dst*3 + 1] = (unsigned char)(g * 255);
            buf[dst*3 + 2] = (unsigned char)(b * 255);
        }
    }
    Magick::Image image;
    image.read(w, h, "RGB", Magick::CharPixel, buf.data());
    image.write(filename);
}




Raytracer::Raytracer(){
    
    t1 = std::chrono::steady_clock::now();

    dpi = 72;
    width = 640;
    height = 480;

    n = width*height;
   

    aadepth = 2;
    aathreshold = 0.1;
    aspectratio = (double)width/ (double)height;
    ambientlight = 0.35;
    accuracy = 0.00000000000000001;
    O = Vect(0,0,0);
    X = Vect(1,0,0);
    Y = Vect(0,1,0);
    Z = Vect(0,0,1);

    campos = Vect(-3,6, -10);
    look_at = Vect(0,0,0);
    diff_btw = Vect(campos.getX() - look_at.getX(), campos.getY() - look_at.getY(), campos.getZ() - look_at.getZ());
    
    camdir = diff_btw.negative().normalize();
    camright = Y.crossProduct(camdir).normalize();
    camdown = camright.crossProduct(camdir);
    scene_cam = Camera(campos, camdir, camright, camdown);

    
}
