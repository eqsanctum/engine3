/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/
#include "ref/WeakReference.h"

#include "../thread/Locker.h"

#include "Object.h"

void Object::acquireWeak(WeakReference<Object*>* ref) {
	Locker locker(&referenceMutex);

	weakReferences.add(ref);
}

void Object::releaseWeak(WeakReference<Object*>* ref) {
	Locker locker(&referenceMutex);

	for (int i = 0; i < weakReferences.size(); ++i) {
		WeakReference<Object*>* reference = weakReferences.get(i);

		if (reference == ref) {
			weakReferences.remove(i);

			break;
		}
	}
}

void Object::destroy() {
	Locker locker(&referenceMutex);

	while (!weakReferences.isEmpty()) {
		WeakReference<Object*>* ref = weakReferences.get(0);
		ref->releaseObject();
	}

	locker.release();

	delete this;
}

#ifdef TRACE_REFERENCES
void Object::addHolder(void* obj) {
	Locker locker(referenceMutex);

	StackTrace* trace = new StackTrace();
	referenceHolders.put(obj, trace);
}

void Object::removeHolder(void* obj) {
	Locker locker(referenceMutex);

	StackTrace* trace = referenceHolders.get(obj);

	if (trace != NULL) {
		delete trace;
		referenceHolders.drop(obj);
	}
}

void Object::printReferenceHolders() {
	for (int i = 0; i < referenceHolders.size(); ++i) {
		StackTrace* trace = referenceHolders.get(i);

		trace->print();
	}
}
#endif