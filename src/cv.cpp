/******************************************************************************
 * AIOS: A personal assistant that can learn anything you want
 * Copyright (C) 2015 Brett Hodgkins
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/

#include "cv.h"

using namespace std;
using namespace cv;

std::pair <int,int> findImageUsingTemplateMatching(string imageURL, unsigned char* sc, int displayWidth, int displayHeight, double &confidence) 
{
    Mat img_object = imread(imageURL, 1);
    Mat img_scene_4c(cv::Size(displayWidth,displayHeight),CV_8UC4,sc);
    Mat img_scene;
    cvtColor(img_scene_4c, img_scene, CV_RGBA2RGB);

    // Create the result matrix
    Mat result;
    int result_cols =  img_scene.cols - img_object.cols + 1;
    int result_rows = img_scene.rows - img_object.rows + 1;

    result.create(result_cols, result_rows, CV_32FC1);

    // Do the Matching and Normalize
    matchTemplate( img_scene, img_object, result, CV_TM_CCOEFF_NORMED );
    //normalize( result, result, 0, 1, NORM_MINMAX, -1, Mat() );

    // Localizing the best match with minMaxLoc
    double minVal; double maxVal; Point minLoc; Point maxLoc;
    Point matchLoc;

    minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, Mat() );

    matchLoc = maxLoc;
    confidence = maxVal;

    return {matchLoc.x+(img_object.cols/2),matchLoc.y+(img_object.rows/2)};
}

std::pair <int,int> reFindImageUsingTemplateMatching(string imageURL, unsigned char* sc, int displayWidth, int displayHeight, std::pair <int,int> prevLoc, double &confidence) 
{
    Mat img_object = imread(imageURL, 1);
    Mat img_scene_4c(cv::Size(displayWidth,displayHeight),CV_8UC4,sc);
    Mat img_scene;
    cvtColor(img_scene_4c, img_scene, CV_RGBA2RGB);

    Rect myROI(prevLoc.first-(img_object.cols*2), prevLoc.second-(img_object.rows*2), img_object.cols*4, img_object.rows*4);
    Mat croppedImage;
    Mat(img_scene, myROI).copyTo(croppedImage);

    // Create the result matrix
    Mat result;
    int result_cols =  croppedImage.cols - img_object.cols + 1;
    int result_rows = croppedImage.rows - img_object.rows + 1;

    result.create(result_cols, result_rows, CV_32FC1);

    // Do the Matching and Normalize
    matchTemplate( croppedImage, img_object, result, CV_TM_CCOEFF_NORMED );
    //normalize( result, result, 0, 1, NORM_MINMAX, -1, Mat() );

    // Localizing the best match with minMaxLoc
    double minVal; double maxVal; Point minLoc; Point maxLoc;
    Point matchLoc;

    minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, Mat() );

    matchLoc = maxLoc;
    confidence = maxVal;

    return {prevLoc.first-(img_object.cols*2)+matchLoc.x+(img_object.cols/2),prevLoc.second-(img_object.rows*2)+matchLoc.y+(img_object.rows/2)};
}
