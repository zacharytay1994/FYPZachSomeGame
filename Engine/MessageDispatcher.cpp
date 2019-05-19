#include "MessageDispatcher.h"

#include "Entity.h"
#include "EntityQueryHandler.h"
#include <ctime>

void MessageDispatcher::Discharge(Entity*& receiver, const Telegram & msg)
{
	receiver->HandleMessage(msg);
}

std::shared_ptr<MessageDispatcher> MessageDispatcher::Instance()
{
	return std::shared_ptr<MessageDispatcher>();
}

void MessageDispatcher::DispatchMsg(double delay, Entity* sender, int receiver, int msg, void* extraInfo)
{
	// get pointer of receiver through id
	Entity* entityReceiver = sender->entityQueryHandler->GetEntityFromID(receiver);
	// create the telegram
	Telegram telegram = { sender, receiver, msg, 0.0, extraInfo };
	// if no delay, discharge message
	if (delay <= 0.0) {
		Discharge(entityReceiver, telegram);
	}
	else {
		double currentTime = clock() - Clock::begin;
		telegram.dispatchTime = currentTime + delay;
		priorityQ.insert(telegram);
	}
}

// called every world update
void MessageDispatcher::DispatchDelayedMsg()
{
	double currentTime = clock() - Clock::begin;

	while (priorityQ.begin()->dispatchTime < currentTime && priorityQ.begin()->dispatchTime > 0) {
		Telegram telegram = *priorityQ.begin();
		// get receiving entity
		Entity* receiver = telegram.sender->entityQueryHandler->GetEntityFromID(telegram.receiverID);
		Discharge(receiver, telegram);
		priorityQ.erase(priorityQ.begin());
	}
}
