//  Body.cpp
//  Universe
//
//  Created by Aquilla Sherrock on 11/19/14.
//  Copyright (c) 2014 Aquilla Sherrock. All rights reserved.

#include "Body.h"

unsigned int Body::instanceCount = 0;
const int Body::viewWidth = 2500;
const int Body::viewHeight = 1400;

Body::Body(double radius, float x, float y)
:radius(radius), velocity({0,0}), position({x,y}) {
    mass = std::pow(radius, 5.0);
    setBody();
}

Body::Body(Body& b1, Body& b2) {
    this->position = Body::centerOfMass(b1, b2);
    this->velocity = Body::postCollisionVelocity(b1, b2);
    this->radius = Physics::sphereRadiusFromVolume(b1.getVolume() + b2.getVolume());
    this->mass = b1.mass + b2.mass;
    
    setBody();
}

Body::~Body() {
    instanceCount--;
}

void Body::setBody() {
    dPosition = {position.x / viewWidth, position.y / viewHeight};
    
    if (radius > 45)      segments = 60;
    else if (radius > 35) segments = 36;
    else if (radius > 25) segments = 24;
    else                   segments = 12;
    
    segmentStep = 360.0 / segments;
    
    this->vertexCount = (segments + 1) * 2 + 2;
    
    aspect = viewWidth / (float) viewHeight;
    dradius = 1.0 / viewWidth * radius;
    this->cradius = {
        radius / viewWidth,
        radius / viewHeight
    };
    
    instanceCount++;
}

void Body::setVelocity(double x, double y) {
        velocity.x = x;
        velocity.y = y;
}

void Body::updatePosition(double dt) {
    double xMove = velocity.x * dt;
    double yMove = velocity.y * dt;
    
    if (position.x + radius + xMove > viewWidth || position.x - radius + xMove < -viewWidth) {
        velocity.x = -velocity.x;
        position.x += velocity.x * dt;
    } else {
        position.x += xMove;
    }
    
    if (position.y + radius + yMove > viewHeight || position.y - radius + yMove < -viewHeight) {
        velocity.y = -velocity.y;
        position.y += velocity.y * dt;
    } else {
        position.y += yMove;
    }
    
    dPosition = {
        position.x / viewWidth,
        position.y / viewHeight
    };
}

void Body::accelerate(pair_t force) {
    velocity.x += force.x / mass;
    velocity.y += force.y / mass;
}

bool Body::collidesWith(Body& other) {
    return Physics::hypoteneuse(getDistanceFrom(other)) < (radius + other.radius);
}

pair_t Body::getDistanceFrom(Body& other) {
    return pair_t {
        other.position.x - this->position.x,
        other.position.y - this->position.y
    };
}

double Body::getDistanceFromSurface(Body& other) {
    pair_t distComp = this->getDistanceFrom(other);
    return Physics::hypoteneuse(distComp) - this->radius - other.radius;
}

pair_t Body::getMomentum() {
    return pair_t {
        mass * velocity.x,
        mass * velocity.y
    };
}

void Body::getVertices(float* bufferData) {
    float deg = 0;
    
    bufferData[0] = dPosition.x;
    bufferData[1] = dPosition.y;
    
    for (int i=2; i < (segments+1)*2 + 1; i+=2) {
        float rad = Physics::deg2rad * deg;
        bufferData[i]   = std::cosf(rad) * cradius.x + dPosition.x;
        bufferData[i+1] = std::sinf(rad) * cradius.y + dPosition.y;
        deg += segmentStep;
    }
}

void Body::getIndices(unsigned short* indexData) {
    for (int i=0, j=0; i < segments*3; i+=3, j++) {
        indexData[i]   = 0;
        indexData[i+1] = j+1;
        indexData[i+2] = j+2;
    }
}

short Body::getNumSegments() {
    return this->segments;
}

double Body::getVolume() {
    return (4.0/3.0) * PI * std::pow(radius, 3);
}

double Body::getMass() {
    return this->mass;
}

pair_t Body::getForce(Body& other) {
    pair_t rV = getDistanceFrom(other);
    
    double r = sqrt(rV.x * rV.x + rV.y * rV.y);
    double f = (G * mass * other.mass) / (r * r);
    
    double fx = f * (rV.x) / r;
    double fy = f * (rV.y) / r;
    
    return pair_t {fx, fy};
}

pair_t Body::centerOfMass(Body& b1, Body& b2) {
    double totalMass = b1.getMass() + b2.getMass();
    double x = (b1.position.x * b1.getMass() + b2.position.x * b2.getMass()) / totalMass;
    double y = (b1.position.y * b1.getMass() + b2.position.y * b2.getMass()) / totalMass;
    return pair_t {x, y};
}

pair_t Body::postCollisionVelocity(Body& b1, Body& b2) {
    double totalMass = b1.getMass() + b2.getMass();
    double x = (b1.getMomentum().x + b2.getMomentum().x) / totalMass;
    double y = (b1.getMomentum().y + b2.getMomentum().y) / totalMass;
    return pair_t {x, y};
}

std::string Body::toString() const {
    std::stringstream stm;
    stm << "Pos:\t[" << position.x << ", " << position.y << "]" << std::endl;
    stm << "Radius:\t" << radius << std::endl;
    stm << "Mass:\t" << mass << std::endl;
    return stm.str();
}






