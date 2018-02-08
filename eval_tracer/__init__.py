import sys
import opcode
import _eval_tracer

f = open('trace.out','w')

def tracefunc(frame, event, arg):
    if event == 'call':
        frame.f_trace_opcodes = True
        frame.f_trace_lines = False
        return tracefunc
    if event == 'opcode':
        _opcode, _oparg = frame.f_code.co_code[frame.f_lasti], frame.f_code.co_code[frame.f_lasti+1]
        f.write('%s %d\n' % (opcode.opname[_opcode], _oparg))
        for i in range(_eval_tracer.get_frame_stack_depth(frame)):
            try:
                o = _eval_tracer.get_frame_stack_item(frame, i)
                try:
                    obj_rep = repr(o)
                except:
                    obj_rep = '[[BAD REPR]]'

                f.write('Stack %d:%s, %s\n' % (i, type(o), obj_rep))
            except ValueError:
                f.write('Stack %d: NULL\n')

def enable():
    sys.settrace(tracefunc)
