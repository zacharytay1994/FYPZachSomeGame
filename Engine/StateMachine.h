#pragma once

#include "State.h"

#include <assert.h>
#include <memory>

template <class EntityType>
class StateMachine {
public:
	StateMachine(EntityType* entityOwner) 
		:
		entityOwner(entityOwner),
		currentState(nullptr),
		previousState(nullptr),
		globalState(nullptr)
	{}
	void SetCurrentState(std::shared_ptr<State<EntityType>> state) {
		currentState = state;
		currentState->Enter(entityOwner);
	}
	void SetPreviousState(std::shared_ptr<State<EntityType>> state) {
		previousState = state;
	}
	void SetGlobalState(std::shared_ptr<State<EntityType>> state) {
		globalState = state;
	}
	std::shared_ptr<State<EntityType>> GetCurrentState() {
		return currentState;
	}
	std::shared_ptr<State<EntityType>> GetPreviousState() {
		return previousState;
	}
	std::shared_ptr<State<EntityType>> GetGlobalState() {
		return globalState;
	}
	bool IsInState(std::shared_ptr<State<EntityType>> state) {
		return state == currentState;
	}
	void Update() {
		// if global state exist, execute it
		if (globalState) {
			globalState->Execute(entityOwner);
		}
		if (currentState) {
			currentState->Execute(entityOwner);
		}
	}
	void ChangeState(std::shared_ptr<State<EntityType>> newState) {
		assert(newState&&"trying to change to a null state");
		// set previous state to current state
		previousState = currentState;
		// exit current state
		currentState->Exit(entityOwner);
		// set current state to new state
		currentState = newState;
		// enter new state
		currentState->Enter(entityOwner);
	}
	void RevertState() {
		ChangeState(previousState);
	}
private:
	// NOTE : entityOwner's destroy method is handled by EntityHandler with a smart pointer, no destroy is necessary here
	// pointer to entity that owns this
	EntityType* entityOwner;
	// current state entity is in
	std::shared_ptr<State<EntityType>> currentState;
	// last state entity was in
	std::shared_ptr<State<EntityType>> previousState;
	// global state of entity to be called every update
	std::shared_ptr<State<EntityType>> globalState;
};
