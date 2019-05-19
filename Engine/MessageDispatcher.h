#pragma once

#include <set>
#include <memory>

class Entity;
class MessageDispatcher {
public:
	enum class Messages {
		AttackFor1Damage
	};
	struct Telegram {
		bool operator()(const Telegram& lhs, const Telegram& rhs) {
			return lhs < rhs;
		}
		bool operator<(const Telegram& rhs) const {
			return dispatchTime < rhs.dispatchTime;
		}
		Entity* sender;
		int receiverID;
		int enumMessage;
		double dispatchTime;
		void* extraInfo;
	};
private:
	MessageDispatcher() {}
	~MessageDispatcher() = delete;
	void Discharge(Entity*& receiver, const Telegram& msg);
public:
	static std::shared_ptr<MessageDispatcher> Instance();
	void DispatchMsg(double delay, Entity* sender, int receiver, int msg, void* extraInfo);
	// called every world update
	void DispatchDelayedMsg();
private:
	// set used for automatic sorting and avoiding duplicated messages
	std::set<Telegram, Telegram> priorityQ;
};