/*
 * differentiation.cpp
 *
 *  Created on: Aug 17, 2011
 *      Author: ankush
 */

double differential(cv::Point3f real_world_position, cv::Point3f cam_position,
					 double sin_theta, double cos_theta, double sin_phi,
					 double cos_phi, int num, int denom)
{
	/*
	 * Returns the value of d(num)/d(denom)
	 *
	 * real_world_position : position of the point in real world frame
	 * cam_position  	   : position of the camera in real world frame
	 * sin_phi			   : sin(pan+heading)
	 * sin_phi			   : cos(pan+heading)
	 * sin_theta           : sin(tilt)
	 * cos_theta		   : cos(tilt)
	 * num				   : Indicator for the numerator.
	 * 						 Can take only 3 values {1,2,3} <==>
	 * 						 {x_cam_world, y_cam_world, z_cam_world}
	 * denom			   : Indicator for denominator
	 * 						 Can take only 5 values {1,2,3,4,5} <==>
	 * 						 {x_cam, y_cam, z_cam, phi, theta}
	 */

	switch (num)
	{
		case 1: // x_cam_world
			switch(denom)
			{
				case 1: // x_cam
					return -1*sin_phi;
					break;
				case 2: // y_cam
					return cos_phi;
					break;
				case 3: // z_cam
					return 0;
					break;
				case 4: // phi
					return cos_phi*(real_world_position.x - cam_position.x)
						   +sin_phi*(real_world_position.y - cam_position.y);
					break;
				case 5: // theta
					return 0;
					break;
			}

		case 2: // y_cam_world
			switch(denom)
			{
				case 1: // x_cam
					return -1.0*cos_phi*sin_theta;
					break;
				case 2: // y_cam
					return -1.0*sin_phi*sin_theta;
					break;
				case 3: // z_cam
					return cos_theta;
					break;
				case 4: // phi
					return -1.0*sin_phi*sin_theta *
							(real_world_position.x - cam_position.x)
						   + cos_phi*sin_theta *
							 (real_world_position.y - cam_position.y);
					break;
				case 5: // theta
					return cos_phi*cos_theta*
							(real_world_position.x - cam_position.x)
							+ sin_phi*cos_theta*
							(real_world_position.y - cam_position.y)
							+ sin_theta*
							(real_world_position.z - cam_position.z);
					break;
			}

		case 3: // z_cam_world
			switch(denom)
			{
				case 1: // x_cam
					return -1.0*cos_phi*cos_theta;
					break;
				case 2: // y_cam
					return -1.0*sin_phi*cos_theta;
					break;
				case 3: // z_cam
					return -1.0*cos_theta;
					break;
				case 4: // phi
					return -1.0*sin_phi*cos_theta *
							(real_world_position.x - cam_position.x)
						   + cos_phi*cos_theta *
							 (real_world_position.y - cam_position.y);
					break;
				case 5: // theta
					return  -1.0*cos_phi*sin_theta*
							(real_world_position.x - cam_position.x)
							+ -1.0*sin_phi*sin_theta*
							(real_world_position.y - cam_position.y)
							+ cos_theta*
							(real_world_position.z - cam_position.z);
					break;
			}
	}
}

double differentiate_image_coor(camera particle_cam, cv::Point3f cam_world_position,
						cv::Point3f real_world_position, int x_or_y, int denom)
{
	/* Calculates the differential of an IMAGE PLANE coordinate, which
	 * can be either x or y, with respect to one of the five camera parameters.
	 *
	 * for differential of x: x_or_y = 1
	 * for differential of y: x_or_y = 2
	 *
	 * denom specifies which camera paramters is the differentiation carried
	 * out:
	 * 	{x_cam, y_cam, z_cam, phi, theta} <=> denom = {1,2,3,4,5}
	 *
	 *  Note:
	 *
	 *  	x_img_plane = (X_cam_world/Z_cam_world*fx) + cx
	 *  		=> d(x_img_plane)/d{} = fx*(ZX' - XZ'/ Z^2)
	 *
	 *  	y_img_plane = (Y_cam_world/Z_cam_world*fy) + cy
	 *			=> d(y_img_plane)/d{} = fy*(YX' - YZ'/ Z^2)
	 */

	double sin_theta = sin(particle_cam.tilt), cos_theta = cos(particle_cam.tilt);
	double sin_phi = sin(particle_cam.theta + particle_cam.pan),
		   cos_phi = cos(particle_cam.theta + particle_cam.pan);

	double D_zCamWorld_by_D_denom = differential(real_world_position,
												particle_cam.position, sin_theta,
												cos_theta, sin_phi, cos_phi, 3,
												denom);
	switch(x_or_y)
	{
		case 1: // x_img_point
		{
			double D_xCamWorld_by_D_denom = differential(real_world_position,
												particle_cam.position, sin_theta,
												cos_theta, sin_phi, cos_phi, 1,
												denom);

			double Z = cam_world_position.z;
			double X = cam_world_position.x;

			double diff_value = 0;
			if(Z != 0)
				diff_value = particle_cam.intrinsics.fx*((Z*D_xCamWorld_by_D_denom
								- X*D_zCamWorld_by_D_denom)/ (Z*Z));
			return diff_value;
			break;
		}
		case 2: // y_img_point
		{
			double D_yCamWorld_by_D_denom = differential(real_world_position,
												particle_cam.position, sin_theta,
												cos_theta, sin_phi, cos_phi, 2,
												denom);
					double Z = cam_world_position.z;
			double Y = cam_world_position.y;

			double diff_value = 0;

			if(Z!=0)
				diff_value = particle_cam.intrinsics.fy*((Z*D_yCamWorld_by_D_denom
								- Y*D_zCamWorld_by_D_denom)/ (Z*Z));
			return diff_value;
			break;
		}
	}
}







double differentiate_costFunction_by_cameraParameter0(line_segment_2d lineSegment_imagePlane,
									  line_segment_3d lineSegment_realWorld,
									  line_segment_3d lineSegment_camWorld,
									  line_segment_2d matching_line, int parameter,
									  camera particle_cam, variances vars)
{
	/* Differentiates the QUADRATIC cost function:
	 *		(x0-x)^2 + (y0-y)^2 + (l0-l)^2 + (theta0-theta)^2
	 * with respect to one parameter of the camera pose as specified by the value of parameter.
	 * Where parameter = {1,2,3,4,5} <=> {x_cam, y_cam, z_cam, phi, theta}
	 *
	 * Arguments:
	 * lineSegment_imagePlane: 2d line segment coordinates in the particle's
	 * 						   image plane
	 * lineSegment_realWorld : 3d coordinates of the same line in real-world
	 *                         frame
	 * lineSegment_camWorld  : 3d coordinates of the same line in camera
	 * 						   world frame
	 * matching_line		 : 2d coordinates of the line in the actual view
	 * 						   which matches the line in particles image plane
	 * particle_cam			 : particle's camera
	 * vars					 : variances struct type, holds the variances for
	 * 						   x_center, y_center, angle, length
	 */

	//double  Zavg = (double)lineSegment_camWorld.pt1.z + (double)lineSegment_camWorld.pt2.z;

	double x0 = (matching_line.pt1.x + matching_line.pt2.x)/2.0,
		   y0 = (matching_line.pt1.y + matching_line.pt2.y)/2.0,
		   len0 = norm(matching_line.pt1 - matching_line.pt2),
		   theta0 = atan((matching_line.pt2.y- matching_line.pt1.y)/
						 (matching_line.pt2.x- matching_line.pt1.x));

	double x_center = (lineSegment_imagePlane.pt1.x +
					   lineSegment_imagePlane.pt2.x)/2.0,
		   y_center = (lineSegment_imagePlane.pt1.y +
				       lineSegment_imagePlane.pt2.y)/2.0,
		   len = norm(lineSegment_imagePlane.pt1-lineSegment_imagePlane.pt2),
		   theta = atan((lineSegment_imagePlane.pt2.y-lineSegment_imagePlane.pt1.y)/
						 (lineSegment_imagePlane.pt2.x-lineSegment_imagePlane.pt1.x));

	double return_val = 0;

	/*
		dpt[1 or 2][x or y]_by_dP : represents the value of
	                              d(pt[1 or 2].[x or y])/dParameter
	 */

	double dpt1X_by_dP = differentiate_image_coor(particle_cam, lineSegment_camWorld.pt1,
												  lineSegment_realWorld.pt1, 1, parameter);
	double dpt2X_by_dP = differentiate_image_coor(particle_cam, lineSegment_camWorld.pt2,
				                                  lineSegment_realWorld.pt2, 1, parameter);
	double dpt1Y_by_dP = differentiate_image_coor(particle_cam, lineSegment_camWorld.pt1,
				                                  lineSegment_realWorld.pt1, 2, parameter);
	double dpt2Y_by_dP = differentiate_image_coor(particle_cam, lineSegment_camWorld.pt2,
				                                  lineSegment_realWorld.pt2, 2, parameter);

	// Center X
	return_val += (x_center - x0)*(dpt1X_by_dP + dpt2X_by_dP)/vars.x;

	// Center Y
	return_val += (y_center - y0)*(dpt1Y_by_dP + dpt2Y_by_dP)/vars.y;

	// Angle
	double delta_y = (lineSegment_imagePlane.pt2.y-lineSegment_imagePlane.pt1.y),
		   delta_x = (lineSegment_imagePlane.pt2.x-lineSegment_imagePlane.pt1.x);
	if(delta_x !=0)
		return_val += (theta - theta0)*(1.0/(1.0 + pow((delta_y/delta_x),2)))*
						(((delta_x*(dpt2Y_by_dP - dpt1Y_by_dP))-
								(delta_y*(dpt2X_by_dP-dpt1X_by_dP)))/(delta_x*delta_x))/vars.angle;
	// Length
	//if(delta_y !=0  )
	return_val += ((len - len0)/len)*2.0*((delta_x*(dpt2X_by_dP-dpt1X_by_dP))
					+ delta_y*(dpt2Y_by_dP - dpt1Y_by_dP))/vars.len;

	return return_val;
}

double differentiate_costFunction_by_cameraParameter(line_segment_2d lineSegment_imagePlane,
									  line_segment_3d lineSegment_realWorld,
									  line_segment_3d lineSegment_camWorld,
									  line_segment_2d matching_line, int parameter,
									  camera particle_cam, variances vars)
{
	/* Differentiates the cost function:
	 *		e^-(x0-x)^2 + e^-(y0-y)^2 + e^-(l0-l)^2 + e^-(theta0-theta)^2
	 * with respect to one parameter of the camera pose as specified by the value of parameter.
	 * Where parameter = {1,2,3,4,5} <=> {x_cam, y_cam, z_cam, phi, theta}
	 *
	 * Arguments:
	 * lineSegment_imagePlane: 2d line segment coordinates in the particle's
	 * 						   image plane
	 * lineSegment_realWorld : 3d coordinates of the same line in real-world
	 *                         frame
	 * lineSegment_camWorld  : 3d coordinates of the same line in camera
	 * 						   world frame
	 * matching_line		 : 2d coordinates of the line in the actual view
	 * 						   which matches the line in particles image plane
	 * particle_cam			 : particle's camera
	 * vars					 : variances struct type, holds the variances for
	 * 						   x_center, y_center, angle, length
	 */

	double x0 = (matching_line.pt1.x + matching_line.pt2.x)/2.0,
		   y0 = (matching_line.pt1.y + matching_line.pt2.y)/2.0,
		   len0 = norm(matching_line.pt1 - matching_line.pt2),
		   theta0 = atan((matching_line.pt2.y- matching_line.pt1.y)/
						 (matching_line.pt2.x- matching_line.pt1.x));

	double x_center = (lineSegment_imagePlane.pt1.x +
					   lineSegment_imagePlane.pt2.x)/2.0,
		   y_center = (lineSegment_imagePlane.pt1.y +
				       lineSegment_imagePlane.pt2.y)/2.0,
		   len = norm(lineSegment_imagePlane.pt1-lineSegment_imagePlane.pt2),
		   theta = atan((lineSegment_imagePlane.pt2.y-lineSegment_imagePlane.pt1.y)/
						 (lineSegment_imagePlane.pt2.x-lineSegment_imagePlane.pt1.x));

	double return_val = 0;

	/*
		dpt[1 or 2][x or y]_by_dP : represents the value of
	                              d(pt[1 or 2].[x or y])/dParameter
	 */

	double dpt1X_by_dP = differentiate_image_coor(particle_cam, lineSegment_camWorld.pt1,
												  lineSegment_realWorld.pt1, 1, parameter);
	double dpt2X_by_dP = differentiate_image_coor(particle_cam, lineSegment_camWorld.pt2,
				                                  lineSegment_realWorld.pt2, 1, parameter);
	double dpt1Y_by_dP = differentiate_image_coor(particle_cam, lineSegment_camWorld.pt1,
				                                  lineSegment_realWorld.pt1, 2, parameter);
	double dpt2Y_by_dP = differentiate_image_coor(particle_cam, lineSegment_camWorld.pt2,
				                                  lineSegment_realWorld.pt2, 2, parameter);

	// Center X
	return_val += 1.0/(pow(2*pi*vars.x,0.5))*exp(-1.0*(x_center-x0)*(x_center-x0)/(2*vars.x))*
				  (1/(2.0*vars.x))*(x0 - x_center)*
				  (dpt1X_by_dP + dpt2X_by_dP);

	// Center Y
	return_val += 1.0/(pow(2*pi*vars.y,0.5))*exp(-1.0*(y_center-y0)*(y_center-y0)/(2*vars.y))*
				  (1/(2.0*vars.y))*(y0 - y_center)*
				  (dpt1Y_by_dP + dpt2Y_by_dP);

	// Angle
	double delta_y = (lineSegment_imagePlane.pt2.y-lineSegment_imagePlane.pt1.y),
		   delta_x = (lineSegment_imagePlane.pt2.x-lineSegment_imagePlane.pt1.x);
	if(delta_x !=0)
		return_val += 1.0/(pow(2*pi*vars.angle,0.5))*exp(-1.0*(theta-theta0)*(theta-theta0)/(2*vars.angle))*
							(1.0/(2.0*vars.angle))*(2.0*(theta0 -theta))*
							(1.0/(1.0 + (delta_y/delta_x)*(delta_y/delta_x)))*
							(((delta_x*(dpt2Y_by_dP - dpt1Y_by_dP))-
							(delta_y*(dpt2X_by_dP-dpt1X_by_dP)))/(delta_x*delta_x));

	// Length
	//if(delta_y !=0  )
	return_val += 1.0/(pow(2*pi*vars.len,0.5))*exp(-1.0*(len0-len)*(len0-len)/(2*vars.len))*
			  (1.0/(2.0*vars.len))*((len0-len)/len)*
			  2.0*((delta_x*(dpt2X_by_dP-dpt1X_by_dP)) +
					delta_y*(dpt2Y_by_dP - dpt1Y_by_dP));

	return return_val;
}



double differentiate_costFunction_by_cameraParameter2(line_segment_2d lineSegment_imagePlane,
									  line_segment_3d lineSegment_realWorld,
									  line_segment_3d lineSegment_camWorld,
									  line_segment_2d matching_line, int parameter,
									  camera particle_cam, variances vars)
{
	/* Differentiates the cost function:
	 *		e^-(x0-x)^2 * e^-(y0-y)^2 * e^-(l0-l)^2 * e^-(theta0-theta)^2
	 * with respect to one parameter of the camera pose as specified by the value of parameter.
	 * Where parameter = {1,2,3,4,5} <=> {x_cam, y_cam, z_cam, phi, theta}
	 *
	 * Arguments:
	 * lineSegment_imagePlane: 2d line segment coordinates in the particle's
	 * 						   image plane
	 * lineSegment_realWorld : 3d coordinates of the same line in real-world
	 *                         frame
	 * lineSegment_camWorld  : 3d coordinates of the same line in camera
	 * 						   world frame
	 * matching_line		 : 2d coordinates of the line in the actual view
	 * 						   which matches the line in particles image plane
	 * particle_cam			 : particle's camera
	 * vars					 : variances struct type, holds the variances for
	 * 						   x_center, y_center, angle, length
	 */

	double x0 = (matching_line.pt1.x + matching_line.pt2.x)/2.0,
		   y0 = (matching_line.pt1.y + matching_line.pt2.y)/2.0,
		   len0 = norm(matching_line.pt1 - matching_line.pt2),
		   theta0 = atan((matching_line.pt2.y- matching_line.pt1.y)/
						 (matching_line.pt2.x- matching_line.pt1.x));

	double x_center = (lineSegment_imagePlane.pt1.x +
					   lineSegment_imagePlane.pt2.x)/2.0,
		   y_center = (lineSegment_imagePlane.pt1.y +
				       lineSegment_imagePlane.pt2.y)/2.0,
		   len = norm(lineSegment_imagePlane.pt1-lineSegment_imagePlane.pt2),
		   theta = atan((lineSegment_imagePlane.pt2.y-lineSegment_imagePlane.pt1.y)/
						 (lineSegment_imagePlane.pt2.x-lineSegment_imagePlane.pt1.x));

	double return_val = 0;

	/*
		dpt[1 or 2][x or y]_by_dP : represents the value of
	                              d(pt[1 or 2].[x or y])/dParameter
	 */

	double dpt1X_by_dP = differentiate_image_coor(particle_cam, lineSegment_camWorld.pt1,
												  lineSegment_realWorld.pt1, 1, parameter);
	double dpt2X_by_dP = differentiate_image_coor(particle_cam, lineSegment_camWorld.pt2,
				                                  lineSegment_realWorld.pt2, 1, parameter);
	double dpt1Y_by_dP = differentiate_image_coor(particle_cam, lineSegment_camWorld.pt1,
				                                  lineSegment_realWorld.pt1, 2, parameter);
	double dpt2Y_by_dP = differentiate_image_coor(particle_cam, lineSegment_camWorld.pt2,
				                                  lineSegment_realWorld.pt2, 2, parameter);

	double delta_y = (lineSegment_imagePlane.pt2.y-lineSegment_imagePlane.pt1.y),
		   delta_x = (lineSegment_imagePlane.pt2.x-lineSegment_imagePlane.pt1.x);


	//std::cout<<"differentiating"<<std::endl;

	// Center X
	if(delta_x != 0)
	{

		double e_power = exp(-1.0*(x_center-x0)*(x_center-x0)/(2*vars.x)
				 -1.0*(y_center-y0)*(y_center-y0)/(2*vars.y)
				 -1.0*(theta-theta0)*(theta-theta0)/(2*vars.angle)/(2.0*vars.angle)
				 -1.0*(len0-len)*(len0-len)/(2*vars.len));

		//std::cout<<"Exponential: "<<e_power<<std::endl;

		return_val = (1.0/(pow(2*pi*vars.x*2*pi*vars.angle*2*pi*vars.y*2*pi*vars.len,0.5)))
					*e_power
					* (
						(x0 - x_center)*(dpt1X_by_dP + dpt2X_by_dP)*(1/(2.0*vars.x))+
						(y0 - y_center)*(dpt1Y_by_dP + dpt2Y_by_dP)*(1/(2.0*vars.y))+
						(2.0*(theta0 -theta))*(1.0/(1.0 + (delta_y/delta_x)*(delta_y/delta_x)))*
											  (((delta_x*(dpt2Y_by_dP - dpt1Y_by_dP))-(delta_y*(dpt2X_by_dP-dpt1X_by_dP)))/(delta_x*delta_x))+
						((len0-len)/len)*2.0*((delta_x*(dpt2X_by_dP-dpt1X_by_dP)) + delta_y*(dpt2Y_by_dP - dpt1Y_by_dP))*(1.0/(2.0*vars.len))

					  );
	}
	else
		std::cout<<"delta_x =0"<<std::endl;

	return return_val;
}


double differentiate_zPull( line_segment_3d lineSegment_realWorld,
							line_segment_3d lineSegment_camWorld,
							int parameter, camera particle_cam,
							variances vars, double z_pullDist = 10.0)
{
	/* Differentiates the cost function:
	 *		e^-((z0-z_average)^2/2*sigma^2)
	 * with respect to one parameter of the camera pose as specified by the value of parameter.
	 * Where parameter = {1,2,3,4,5} <=> {x_cam, y_cam, z_cam, phi, theta}
	 *
	 * Arguments:
	 * lineSegment_camWorld  : 3d coordinates of the line in camera world frame
	 * matching_line		 : 2d coordinates of the line in the actual view
	 * 						   which matches the line in particles image plane
	 * particle_cam			 : particle's camera
	 * vars					 : variances struct type, holds the variances for
	 * 						   x_center, y_center, angle, length
	 */

	double sin_theta = sin(particle_cam.tilt), cos_theta = cos(particle_cam.tilt);
	double sin_phi = sin(particle_cam.theta + particle_cam.pan),
		   cos_phi = cos(particle_cam.theta + particle_cam.pan);

	double D_z_pt1CamWorld_by_D_parameter = differential(lineSegment_realWorld.pt1,
												particle_cam.position, sin_theta,
												cos_theta, sin_phi, cos_phi, 3,
												parameter),
		   D_z_pt2CamWorld_by_D_parameter = differential(lineSegment_realWorld.pt2,
			   									particle_cam.position, sin_theta,
												cos_theta, sin_phi, cos_phi, 3,
												parameter);

	double lineSegment_z_avg = (lineSegment_camWorld.pt1.z +lineSegment_camWorld.pt2.z)/2;

	// Z pulling into the visible area
	double slope = 1.0/(pow(2*pi*vars.z_front,0.5))
			*exp(-1.0*(z_pullDist-lineSegment_z_avg)*(z_pullDist-lineSegment_z_avg)/(2*vars.z_front))
			*(1/(2.0*vars.z_front))*(z_pullDist-lineSegment_z_avg)
			*(D_z_pt2CamWorld_by_D_parameter + D_z_pt1CamWorld_by_D_parameter);

	std::cout<<"z_parameter: "<<parameter<<std::endl;
	std::cout<<"z slopes: "<<D_z_pt2CamWorld_by_D_parameter + D_z_pt1CamWorld_by_D_parameter<<std::endl;
	std::cout<<"Zpull delta z: "<<z_pullDist-lineSegment_z_avg<<"  slope: "<<slope<<std::endl;
	return slope;
}
