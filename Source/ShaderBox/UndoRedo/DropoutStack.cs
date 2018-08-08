using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ShaderBox.UndoRedo
{
    public class DropOutStack<T>
    {
        private T[] _items;
        private int _top = 0;
        private int _itemCount = 0;
        private int _capacity;

        public DropOutStack(int capacity)
        {
            _capacity = capacity;
            _items = new T[capacity];
        }

        public void Push(T item)
        {
            ++_itemCount;
            _items[_top] = item;
            _top = (_top + 1) % _items.Length;
        }
        public T Pop()
        {
            if(_itemCount == 0)
            {
                return default(T);
            }
            --_itemCount;

            _top = (_items.Length + _top - 1) % _items.Length;

            T item = _items[_top];
            _items[_top] = default(T);

            return item;
        }

        public void Clear()
        {
            _itemCount = 0;
            _top = 0;
            for(int i = 0; i < _capacity; ++i)
            {
                _items[i] = default(T);
            }
        }
    }
}