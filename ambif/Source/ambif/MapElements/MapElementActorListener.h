// Stefano Cherubin stefano1.cherubin@mail.polimi.it - except where otherwise stated

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
