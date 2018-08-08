namespace ShaderBox.UndoRedo
{
    public static class UndoRedoSystem
    {
        private static UndoRedoStack ActiveStack
        {
            get
            {
                if(CustomStack != null)
                {
                    return CustomStack;
                }
                return _mainStack;
            }
        }

        public static void Undo()
        {
            ActiveStack.Undo();
        }

        public static void Redo()
        {
            ActiveStack.Redo();
        }

        public static void AddToUndoStack(IUndoRedoItem item)
        {
            ActiveStack.AddToUndoStack(item);
        }

        public static UndoRedoStack CustomStack { private get; set; }

        private static UndoRedoStack _mainStack = new UndoRedoStack();
    }

    public class UndoRedoStack
    {
        private const int MAX_STACK_SIZE = 32;
        private bool _isChanging = false;
        DropOutStack<IUndoRedoItem> _undoStack = new DropOutStack<IUndoRedoItem>(MAX_STACK_SIZE);
        DropOutStack<IUndoRedoItem> _redoStack = new DropOutStack<IUndoRedoItem>(MAX_STACK_SIZE);

        public void AddToUndoStack(IUndoRedoItem item)
        {
            if (!_isChanging)
            {
                _undoStack.Push(item);
                _redoStack.Clear();
            }
        }

        public void Undo()
        {
            _isChanging = true;
            IUndoRedoItem item = _undoStack.Pop();
            if (item != null)
            {
                item.Undo();
                _redoStack.Push(item);
            }
            _isChanging = false;
        }

        public void Redo()
        {
            _isChanging = true;
            IUndoRedoItem item = _redoStack.Pop();
            if (item != null)
            {
                item.Redo();
                _undoStack.Push(item);
            }
            _isChanging = false;
        }
    }
}
