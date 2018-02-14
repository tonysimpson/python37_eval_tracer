import sys
import opcode
import time
import _eval_tracer


class Trace:
    __slots__ = 'timestamp code_index stack_types globals_version builtins_version'.split()
    
    def __init__(self, timestamp, code_index, stack_types, globals_version, builtins_version):
        self.timestamp = timestamp
        self.code_index = code_index
        self.stack_types = stack_types
        self.globals_version = globals_version
        self.builtins_version = builtins_version


class FrameTrace:
    def __init__(self, frame, stack_types_cache):
        self.frame = frame
        self.trace = []
        self.child_frames = []
        self.stack_types_cache = stack_types_cache

    def __call__(self, frame, event, arg):
        if event == 'call':
            assert frame != self.frame, "Expected call to be a new frame"
            frame.f_trace_opcodes = True
            frame.f_trace_lines = False
            ft = FrameTrace(frame, self.stack_types_cache)
            self.child_frames.append(ft)
            return ft
        if event == 'opcode':
            stack_types = tuple(type(_eval_tracer.get_frame_stack_item(frame, i)) for i in range(_eval_tracer.get_frame_stack_depth(frame)))
            if stack_types in self.stack_types_cache:
                stack_types = self.stack_types_cache[stack_types]
            else:
                self.stack_types_cache[stack_types] = stack_types
            self.trace.append(Trace(time.time(), frame.f_lasti, stack_types, _eval_tracer.dict_version(frame.f_globals), _eval_tracer.dict_version(frame.f_builtins)))
            return self


def enable():
    ft = FrameTrace(sys._getframe(), {})
    sys.settrace(ft)
    return ft


def disable():
    sys.settrace(None)


def visit_frames(f):
    for cf in f.child_frames:
        yield from visit_frames(cf)
    yield f


