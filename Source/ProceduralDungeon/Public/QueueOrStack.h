// Copyright Benoit Pelletier 2021 - 2025 All Rights Reserved.
//
// This software is available under different licenses depending on the source from which it was obtained:
// - The Fab EULA (https://fab.com/eula) applies when obtained from the Fab marketplace.
// - The CeCILL-C license (https://cecill.info/licences/Licence_CeCILL-C_V1-en.html) applies when obtained from any other source.
// Please refer to the accompanying LICENSE file for further details.

#pragma once

#include "Containers/Queue.h"
#include "Containers/Array.h"

template<typename T>
class TQueueOrStack
{
public:
	enum class EMode { QUEUE, STACK };

	TQueueOrStack(EMode _Mode = EMode::QUEUE)
		: Mode(_Mode), Queue(), Stack()
	{
	}

	// Sets the mode (queue or stack) and clears its content.
	void SetMode(EMode _Mode)
	{
		Empty();
		Mode = _Mode;
	}

	void Push(T& Element)
	{
		switch (Mode)
		{
		case EMode::QUEUE:
			Queue.Enqueue(Element);
			break;
		case EMode::STACK:
			Stack.Push(Element);
			break;
		}
	}

	T Pop()
	{
		check(!IsEmpty());
		T item = T();
		switch (Mode)
		{
		case EMode::QUEUE:
			Queue.Dequeue(item);
			break;
		case EMode::STACK:
			item = Stack.Pop();
			break;
		}
		return item;
	}

	bool IsEmpty()
	{
		switch (Mode)
		{
		case EMode::QUEUE:
			return Queue.IsEmpty();
		case EMode::STACK:
			return Stack.Num() <= 0;
		}
		return true;
	}

	void Empty()
	{
		Queue.Empty();
		Stack.Empty();
	}

private:
	EMode Mode;
	TQueue<T> Queue;
	TArray<T> Stack;
};
