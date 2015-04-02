//* Copyright 2015 Stefano Cherubin
//* stefano1{dot}cherubin{at}mail{dot}polimi{dot}it
//* ISPG - Image and Sound Processing Group
//* Politecnico di Milano

#pragma once

/**
 * Interface for delivering mouse event notifications from AMapElementActor to listeners
 *
 * Known implementations:
 * - AMapElementsManager
 *
 * @see AMapElementActor
 * @see AMapElementsManager
 */
class AMBIF_API MapElementActorListener
{
public:

	virtual void OnMouseBeginHover(AActor* TouchedComponent) {};

	virtual void OnMouseEndHover(AActor* TouchedComponent) {};

	virtual void OnMouseClick(AActor* TouchedComponent) {};
};
