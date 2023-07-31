/*
 * MIT License
 *
 * Copyright (c) 2021 Benoit Pelletier
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include "Containers/Queue.h" 	
#include "Containers/Array.h"

template<typename T>
class TQueueOrStack
{
public:
	enum class EMode { QUEUE, STACK };

	TQueueOrStack(EMode _Mode)
		: Mode(_Mode), Queue(), Stack()
	{
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

	int Num()
	{
		switch (Mode)
		{
		case EMode::QUEUE:
			return Queue.Num();
		case EMode::STACK:
			return Stack.Num();
		}
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

private:
	EMode Mode;
	TQueue<T> Queue;
	TArray<T> Stack;
};
