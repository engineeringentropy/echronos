from prj import Module


class AcruxModule(Module):
    xml_schema = """
  <schema>
   <entry name="taskid_size" type="int" default="8"/>
   <entry name="irqeventid_size" type="int" default="8"/>
   <entry name="prefix" type="c_ident" default="rtos_" />
   <entry name="tasks" type="list" auto_index_field="idx">
     <entry name="task" type="dict">
      <entry name="entry" type="c_ident" />
      <entry name="name" type="c_ident" />
      <entry name="stack_size" type="int" />
     </entry>
   </entry>
   <entry name="irq_events" type="list" auto_index_field="idx">
     <entry name="irq_event" type="dict">
      <entry name="name" type="c_ident" />
     </entry>
   </entry>
   <entry name="mutexes" type="list" auto_index_field="idx">
     <entry name="mutex" type="dict">
      <entry name="name" type="c_ident" />
     </entry>
   </entry>
  </schema>
"""
    files = [
        {'input': 'rtos-acrux.h', 'render': True},
        {'input': 'rtos-acrux.c', 'render': True, 'type': 'c'},
    ]

module = AcruxModule()
