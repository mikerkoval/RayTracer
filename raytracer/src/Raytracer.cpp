#include "Raytracer.h"
#include "Sphere.h"
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
    return Color(0, 0, 0, 0);
}

Color Raytracer::getColorAt(Vect intersection_position,Vect intersecting_direction, vector<Object*> scene_objects, int index_closest,vector<Source*> light_sources,double  accuracy,double ambientlight, int n){
  
 

    scene_objects.at(index_closest);

    // Emissive objects return their emission color immediately
    Color emission = scene_objects.at(index_closest)->emission;
    if (emission.getRed() > 0 || emission.getGreen() > 0 || emission.getBlue() > 0) {
        return emission;
    }

    Color winning_object_color = scene_objects.at(index_closest)->getColor(intersection_position);
    Vect winning_object_normal = scene_objects.at(index_closest)->getNormalAt(intersection_position);

    // Normal map perturbation
    if (scene_objects.at(index_closest)->normalMap) {
        // Get UV for this hit point
        double u = 0, v = 0;
        Sphere* sp = dynamic_cast<Sphere*>(scene_objects.at(index_closest));
        if (sp) sp->getUV(intersection_position, u, v);

        // Sample tangent-space normal from map
        Vect tn = scene_objects.at(index_closest)->normalMap->sample(u, v);

        // Build TBN: tangent frame aligned to the surface normal
        Vect N = winning_object_normal;
        Vect up = (fabs(N.getY()) < 0.9) ? Vect(0,1,0) : Vect(1,0,0);
        Vect T = N.crossProduct(up).normalize();   // tangent
        Vect B = N.crossProduct(T).normalize();    // bitangent

        // Transform tangent-space normal to world space
        winning_object_normal = Vect(
            T.getX()*tn.getX() + B.getX()*tn.getY() + N.getX()*tn.getZ(),
            T.getY()*tn.getX() + B.getY()*tn.getY() + N.getY()*tn.getZ(),
            T.getZ()*tn.getX() + B.getZ()*tn.getY() + N.getZ()*tn.getZ()
        ).normalize();
    }

    // Ambient occlusion: cast rays across the hemisphere, count unblocked fraction
    const int ao_samples = 32;
    const double ao_radius = 0.5;  // max distance an occluder counts
    // Build tangent frame from normal
    Vect ao_up = (fabs(winning_object_normal.getY()) < 0.9) ? Vect(0,1,0) : Vect(1,0,0);
    Vect ao_tan = winning_object_normal.crossProduct(ao_up).normalize();
    Vect ao_bit = winning_object_normal.crossProduct(ao_tan).normalize();
    // Per-point rotation offset so the sample pattern differs at every surface point
    double ao_rot = fmod(
        intersection_position.getX() * 127.1 +
        intersection_position.getY() * 311.7 +
        intersection_position.getZ() * 74.7, 1.0) * 2.0 * M_PI;
    int ao_unblocked = 0;
    for (int i = 0; i < ao_samples; i++) {
        // Stratified Fibonacci-like hemisphere directions, rotated per surface point
        double fi = (i + 0.5) / ao_samples;
        double phi = acos(sqrt(1.0 - fi));   // elevation: more samples near horizon
        double theta = 2.39996 * i + ao_rot;  // golden angle azimuth + per-point rotation
        double sp = sin(phi), cp = cos(phi);
        double st = sin(theta), ct = cos(theta);
        Vect ao_dir = ao_tan.mult(sp * ct).add(ao_bit.mult(sp * st)).add(winning_object_normal.mult(cp));
        Vect ao_origin = intersection_position.add(winning_object_normal.mult(0.0001));
        Ray ao_ray(ao_origin, ao_dir);
        bool occluded = false;
        for (int oi = 0; oi < (int)scene_objects.size(); oi++) {
            if (oi == index_closest && scene_objects.at(oi)->isConvex()) continue;
            if (scene_objects.at(oi)->transparency > 0.5) continue;
            double t = scene_objects.at(oi)->findIntersection(ao_ray);
            if (t > 0.0001 && t < ao_radius) { occluded = true; break; }
        }
        if (!occluded) ao_unblocked++;
    }
    double ao_factor = pow((double)ao_unblocked / ao_samples, 2.0);

    double specularity = winning_object_color.getSpecularity();
    Color final_color = winning_object_color.scalar(ambientlight * ao_factor);
    Color reflection_color(0, 0, 0, 0);
    bool has_reflection = false;

    if (specularity > 0 && specularity <= 1) {
        // reflection from objects with specular intensity
        double dot1 = winning_object_normal.dotProduct(intersecting_direction.negative());
        Vect scalar1 = winning_object_normal.mult(dot1);
        Vect add1 = scalar1.add(intersecting_direction);
        Vect scalar2 = add1.mult(2);
        Vect add2 = intersecting_direction.negative().add(scalar2);
        Vect reflection_direction = add2.normalize();

        // offset origin along normal to avoid self-intersection
        Vect reflection_ray_origin = intersection_position.add(winning_object_normal.mult(0.001));
        Ray reflection_ray (reflection_ray_origin, reflection_direction);

        vector<double> reflection_intersections;
        for (int reflection_index = 0; reflection_index < (int)scene_objects.size(); reflection_index++) {
            if (reflection_index == index_closest && scene_objects.at(reflection_index)->isConvex()) {
                reflection_intersections.push_back(-1);
            } else {
                reflection_intersections.push_back(scene_objects.at(reflection_index)->findIntersection(reflection_ray));
            }
        }

        int index_of_winning_object_with_reflection = closestObjectIndex(reflection_intersections);

        if (index_of_winning_object_with_reflection == -1) {
            reflection_color = skyColor(reflection_direction);
            has_reflection = true;
        } else if (reflection_intersections.at(index_of_winning_object_with_reflection) > 0.001 && n > 0) {
            Vect reflection_intersection_position = intersection_position.add(reflection_direction.mult(reflection_intersections.at(index_of_winning_object_with_reflection)));
            reflection_color = getColorAt(reflection_intersection_position, reflection_direction, scene_objects, index_of_winning_object_with_reflection, light_sources, accuracy, ambientlight, n-1);
            has_reflection = true;
        }
      

     
         
    }
    // Refraction + Fresnel
    double transparency = scene_objects.at(index_closest)->transparency;
    double ior = scene_objects.at(index_closest)->ior;
    if (transparency > 0 && ior > 0 && n > 0) {
        double cos_i = winning_object_normal.dotProduct(intersecting_direction.negative());
        double n1, n2;
        Vect ref_normal;
        if (cos_i > 0) {
            n1 = 1.0; n2 = ior;
            ref_normal = winning_object_normal;
        } else {
            n1 = ior; n2 = 1.0;
            ref_normal = winning_object_normal.negative();
            cos_i = -cos_i;
        }
        double eta = n1 / n2;
        double sin2_t = eta * eta * (1.0 - cos_i * cos_i);

        // Schlick Fresnel: how much is reflected vs transmitted
        double F0 = (n1 - n2) / (n1 + n2);
        F0 = F0 * F0;
        double fresnel = F0 + (1.0 - F0) * pow(1.0 - cos_i, 5.0);

        if (sin2_t > 1.0) {
            // Total internal reflection — full reflection, no transmission
            fresnel = 1.0;
        } else {
            // Cast refraction ray
            double cos_t = sqrt(1.0 - sin2_t);
            Vect refract_dir = intersecting_direction.mult(eta)
                .add(ref_normal.mult(eta * cos_i - cos_t)).normalize();
            Vect refract_origin = intersection_position.add(ref_normal.negative().mult(0.0001));
            Ray refract_ray(refract_origin, refract_dir);

            vector<double> refract_intersections;
            for (int ri = 0; ri < (int)scene_objects.size(); ri++) {
                refract_intersections.push_back(scene_objects.at(ri)->findIntersection(refract_ray));
            }
            int refract_index = closestObjectIndex(refract_intersections);

            Color refract_color;
            if (refract_index == -1) {
                refract_color = skyColor(refract_dir);
            } else if (refract_intersections.at(refract_index) > 0.0001) {
                Vect refract_pos = refract_origin.add(refract_dir.mult(refract_intersections.at(refract_index)));
                refract_color = getColorAt(refract_pos, refract_dir, scene_objects, refract_index, light_sources, accuracy, ambientlight, n - 1);
            }
            // Blend: fresnel portion reflects, rest transmits
            // Tint: mix a small amount of the surface color into the transmitted result
            double transmit = transparency * (1.0 - fresnel);
            double tint = (1.0 - transparency) * 0.5;
            final_color = refract_color.scalar(transmit)
                .add(winning_object_color.scalar(tint))
                .add(final_color.scalar(1.0 - transmit - tint));
        }

        // Add Fresnel reflection on top for transparent objects
        if (fresnel > 0 && n > 0 && winning_object_color.getSpecularity() <= 0) {
            double dot1 = winning_object_normal.dotProduct(intersecting_direction.negative());
            Vect scalar1 = winning_object_normal.mult(dot1);
            Vect add1 = scalar1.add(intersecting_direction);
            Vect refl_dir = intersecting_direction.negative().add(add1.mult(2)).normalize();
            Vect refl_origin = intersection_position.add(winning_object_normal.mult(0.001));
            Ray refl_ray(refl_origin, refl_dir);

            vector<double> refl_ints;
            for (int ri = 0; ri < (int)scene_objects.size(); ri++) {
                if (ri == index_closest && scene_objects.at(ri)->isConvex()) refl_ints.push_back(-1);
                else refl_ints.push_back(scene_objects.at(ri)->findIntersection(refl_ray));
            }
            int refl_idx = closestObjectIndex(refl_ints);
            Color refl_color;
            if (refl_idx == -1) {
                refl_color = skyColor(refl_dir);
            } else if (refl_ints.at(refl_idx) > 0.001) {
                Vect refl_pos = refl_origin.add(refl_dir.mult(refl_ints.at(refl_idx)));
                refl_color = getColorAt(refl_pos, refl_dir, scene_objects, refl_idx, light_sources, accuracy, ambientlight, n - 1);
            }
            final_color = final_color.add(refl_color.scalar(fresnel * transparency));
        }
    }

    for (int light_index = 0; light_index < light_sources.size(); light_index++){
        Vect light_direction = light_sources.at(light_index) -> getPosition().add(intersection_position.negative()).normalize();
        float cosine_angle = winning_object_normal.dotProduct(light_direction);
        if (cosine_angle < 0) cosine_angle = -cosine_angle; // two-sided lighting
        if(cosine_angle >0 || scene_objects.at(index_closest) ->getCL()){
            //test for shadows 
            
            Vect light_pos = light_sources.at(light_index)->getPosition();
            double light_radius = light_sources.at(light_index)->getRadius();

            Vect distance_to_light = light_pos.add(intersection_position.negative());
            double distance_to_light_magnitude = distance_to_light.magnitude();
            Vect light_dir = distance_to_light.normalize();
            Vect shadow_origin = intersection_position.add(light_dir.mult(0.0001));
            Vect up = (fabs(light_dir.getY()) < 0.9) ? Vect(0,1,0) : Vect(1,0,0);
            Vect tangent   = light_dir.crossProduct(up).normalize();
            Vect bitangent = light_dir.crossProduct(tangent).normalize();

            // Sample the light disk uniformly (point light = 1 sample at center)
            int grid = 5;
            int shadow_samples = 0;
            int unblocked = 0;
            for (int si = 0; si < grid; si++) {
                for (int sj = 0; sj < grid; sj++) {
                    double ou = (grid == 1) ? 0.0 : ((si + 0.5) / grid * 2.0 - 1.0);
                    double ov = (grid == 1) ? 0.0 : ((sj + 0.5) / grid * 2.0 - 1.0);
                    Vect sample_pos = light_pos
                        .add(tangent.mult(ou * light_radius))
                        .add(bitangent.mult(ov * light_radius));
                    Vect sample_vec = sample_pos.add(intersection_position.negative());
                    Vect sample_dir = sample_vec.normalize();
                    double sample_dist = sample_vec.magnitude();

                    // Skip samples behind the surface
                    if (winning_object_normal.dotProduct(sample_dir) <= 0) continue;

                    shadow_samples++;
                    Ray shadow_ray(shadow_origin, sample_dir);
                    bool blocked = false;
                    for (int object_index = 0; object_index < scene_objects.size(); object_index++) {
                        if (object_index == index_closest) continue;
                        if (scene_objects.at(object_index)->getCL()) continue;
                        double t = scene_objects.at(object_index)->findIntersection(shadow_ray);
                        if (t > 0.001 && t <= sample_dist) {
                            blocked = true;
                            break;
                        }
                    }
                    if (!blocked) unblocked++;
                }
            }

            double shadow_factor = (shadow_samples > 0) ? (double)unblocked / shadow_samples : 0.0;

            if (shadow_factor > 0) {
                double falloff = 1.0;
                double li = light_sources.at(light_index)->getIntensity();
                if (li > 0.0) {
                    falloff = li / distance_to_light_magnitude;
                }
                if (scene_objects.at(index_closest)->getCL()) {
                    final_color = final_color.add(winning_object_color.multiply(light_sources.at(light_index)->getColor()).scalar(shadow_factor * falloff));
                } else {
                    final_color = final_color.add(winning_object_color.multiply(light_sources.at(light_index)->getColor()).scalar(cosine_angle * shadow_factor * falloff));

                    // Blinn-Phong specular highlight
                    double shininess = scene_objects.at(index_closest)->shininess;
                    if (shininess > 0) {
                        Vect view_dir = intersecting_direction.negative().normalize();
                        Vect half_vec = light_dir.add(view_dir).normalize();
                        double spec = pow(fmax(0.0, winning_object_normal.dotProduct(half_vec)), shininess);
                        Color spec_color = light_sources.at(light_index)->getColor().scalar(spec * shadow_factor * falloff);
                        final_color = final_color.add(spec_color);
                    }
                }
            }
        }   
    }
    // Blend reflection: mix diffuse and reflected color by specularity
    if (has_reflection) {
        final_color = final_color.scalar(1.0 - specularity)
            .add(reflection_color.scalar(specularity));
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
                        // anti-aliasing — aax offsets x, aay offsets y independently
                        double xoff = (double)aax / ((double)aadepth - 1);
                        double yoff = (double)aay / ((double)aadepth - 1);
                        if (width > height) {
                            xamnt = ((x + xoff) / width) * aspectratio - (((width-height)/(double)height)/2);
                            yamnt = ((height - y) + yoff) / height;
                        }
                        else if (height > width) {
                            xamnt = (x + xoff) / width;
                            yamnt = (((height - y) + yoff) / height) / aspectratio - (((height - width)/(double)width)/2);
                        }
                        else {
                            xamnt = (x + xoff) / width;
                            yamnt = ((height - y) + yoff) / height;
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


    
int Raytracer::generate (vector<Object*> objs, vector<Source*>lights, std::string filename, int aa, Vect cp, Vect cd, bool fast, ToneMap tonemap, double gamma, double ambient){
  
    

    std::cout << "Launched from the main\n";


    ambientlight = ambient;
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
    savepng(filename.c_str(), width, height, dpi, pixels, n, tonemap, gamma);

    delete pixels;
    auto t2 = std::chrono::steady_clock::now();
    double diff = std::chrono::duration<double>(t2 - t1).count();
    cout << diff << " seconds " << endl;
    return 0; 
}
static double applyToneMap(double c, ToneMap tonemap) {
    switch (tonemap) {
        case ToneMap::Reinhard:
            return c / (c + 1.0);
        case ToneMap::ACES: {
            // ACES filmic approximation (Narkowicz 2015), black-point corrected
            const double a = 2.51, b = 0.03, c2 = 2.43, d = 0.59, e = 0.14;
            const double black_lift = b / e;  // value at c=0
            double v = (c * (a * c + b)) / (c * (c2 * c + d) + e);
            return (v - black_lift) / (1.0 - black_lift);
        }
        default:
            return c;
    }
}

void Raytracer::savepng (const char *filename, int w, int h, int dpi, RGBType *data, int size, ToneMap tonemap, double gamma){
    cout << "SAVING \n";
    double inv_gamma = (gamma > 0.0) ? 1.0 / gamma : 1.0;
    // Build raw 8-bit RGB buffer, flipping rows (renderer stores bottom-to-top)
    vector<unsigned char> buf(w * h * 3);
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int src = (h - 1 - y) * w + x;
            int dst = y * w + x;
            double r = applyToneMap(data[src].r, tonemap);
            double g = applyToneMap(data[src].g, tonemap);
            double b = applyToneMap(data[src].b, tonemap);
            // gamma correction
            if (inv_gamma != 1.0) {
                r = pow(fmax(0.0, r), inv_gamma);
                g = pow(fmax(0.0, g), inv_gamma);
                b = pow(fmax(0.0, b), inv_gamma);
            }
            r = r < 0 ? 0 : (r > 1 ? 1 : r);
            g = g < 0 ? 0 : (g > 1 ? 1 : g);
            b = b < 0 ? 0 : (b > 1 ? 1 : b);
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
    width = 1920;
    height = 1080;

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
