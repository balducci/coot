import { MouseEventHandler, useState } from 'react'
// import './_App.css'
import './index.css';
import { Lhasa } from './main.tsx'
import * as d3 from "d3";
import { Canvas, Color } from './lhasa';

class ToolButtonProps {
  onclick: MouseEventHandler<HTMLDivElement> | undefined;
  caption: string | undefined;
  icon: string | undefined
}

function ToolButton(props:ToolButtonProps) {
  // console.log(props.caption);
  return (
    <div className="button tool_button" onClick={props.onclick}>
      {props.caption}
      {/* {props.icon && 
        <>
          <img src={props.icon} width="24px" />
          <br/>
        </>} */}
    </div>
  )
}

export function LhasaComponent() {
  const [lh, setLh] = useState(() => {
    return new Lhasa.Canvas();
  });
  const chLh = (func: () => void) => {
    func();
    setLh(lh);
  };

  function switch_tool(tool : any) {
    // todo: pressed buttons
    console.log('tool switch!');
    chLh(() => lh.set_active_tool(Lhasa.make_active_tool(tool)));
  };
  
  function on_render() {
    console.debug("on_render() called.");
    const css_color_from_lhasa_color = (lhasa_color: Color) => {
      return 'rgba(' + lhasa_color.r * 255 + ','+ lhasa_color.g * 255 + ',' + lhasa_color.b * 255 + ',' + lhasa_color.a + ')';
    }
    const lhasa_text_to_d3js = (msvg, text) => {
      const style_to_attrstring = (style) => {
        let style_string = "";
        if(style.specifies_color) {
          style_string += "fill:";
          style_string += css_color_from_lhasa_color(style.color);
          style_string += ";";
        }
        // font-size:75%;baseline-shift:sub / super
        switch(style.positioning) {
          case Lhasa.TextPositioning.Normal:
            if(style.size != "") {
              style_string += "font-size:";
              style_string += style.size;
              style_string += ";";
            }
            break;
          case Lhasa.TextPositioning.Sub:
            style_string += "font-size:75%;baseline-shift:sub;";
            break;
          case Lhasa.TextPositioning.Super:
            style_string += "font-size:75%;baseline-shift:super;";
            break;
        }
        if(style.weight != "") {
          style_string += "font-weight:";
          style_string += style.weight;
          style_string += ";";
        }
        return style_string;
      };
      const append_spans_to_node = (spans, text_node) => {
        for(let i = 0; i < spans.size(); i++) {
          const span = spans.get(i);
          let child = text_node
            // .enter()
            .append("tspan");
          if(span.specifies_style) {
            child.attr("style", style_to_attrstring(span.style));
          }
          if(span.has_subspans()) {
            append_spans_to_node(span.as_subspans(), child);
          } else {
            const caption = span.as_caption();
            child.text(caption);
          }
        }
        // return ret;
      }
      const ret = msvg.append("text")
        .attr("x", text.origin.x)
        .attr("y", text.origin.y);
      
      const style_string = style_to_attrstring(text.style);
      if(style_string != "") {
        ret.attr("style", style_string);
      } else {
        // console.warn("Empty style string!");
      }
      if(text.spans.size() == 0) {
        console.warn("Text contains no spans!");
      }
      append_spans_to_node(text.spans, ret);
      return ret;
    };
    const text_measure_function = (text) => {
      // let size_info = new Lhasa.TextSize;
      let size_info = {
        'width': 0,
        'height': 0
      };
      try{
        const parent = document.getElementById("editor_canvas_container");
        const msvg = d3.create("svg")
          .attr("width", 100)
          .attr("height", 100)
          .attr("id", "measurement_temporary");
        const text_elem = lhasa_text_to_d3js(msvg, text);
        parent.append(msvg.node());
        const node = text_elem.node();
        // This has awful performance but I don't really have a choice
        const bbox = node.getBBox();
        size_info.width = Math.ceil(bbox.width);
        size_info.height = Math.ceil(bbox.height);
        msvg.remove();
      } catch(err) {
        console.error('Error occured in text measurement: ', err);
      } finally {
        return size_info;
      }
      
    };
    const ren = new Lhasa.Renderer(text_measure_function);
    lh.render(ren);
    const commands = ren.get_commands();
    const svg = d3.create("svg")
      .attr("id", "lhasa_drawing")
      .attr("width", lh.measure(Lhasa.MeasurementDirection.HORIZONTAL).requested_size)
      .attr("height", lh.measure(Lhasa.MeasurementDirection.VERTICAL).requested_size);

    const render_commands = (cmds) => {
      for(var i = 0; i < cmds.size(); i++) {
        const command = cmds.get(i);
        if(command.is_line()) {
          const line = command.as_line();
          const color = line.style.color;
          svg.append("line")
            .attr("x1", line.start.x)
            .attr("y1", line.start.y)
            .attr("x2", line.end.x)
            .attr("y2", line.end.y)
            .attr("stroke", css_color_from_lhasa_color(color))
            .attr("stroke-width", line.style.line_width);

        } else if(command.is_arc()) {
          const arc = command.as_arc();
          // Thanks to: https://stackoverflow.com/questions/5736398/how-to-calculate-the-svg-path-for-an-arc-of-a-circle
          function polarToCartesian(centerX, centerY, radius, angleInRadians) {
            //var angleInRadians = (angleInDegrees-90) * Math.PI / 180.0;

            return {
              x: centerX + (radius * Math.cos(angleInRadians)),
              y: centerY + (radius * Math.sin(angleInRadians))
            };
          }

          function describeArc(x, y, radius, startAngle, endAngle){

              var start = polarToCartesian(x, y, radius, endAngle);
              var end = polarToCartesian(x, y, radius, startAngle);

              var largeArcFlag = endAngle - startAngle <= Math.PI ? "0" : "1";
              //var largeArcFlag = "1";

              var d = [
                  "M", start.x, start.y, 
                  "A", radius, radius, 0, largeArcFlag, 0, end.x, end.y
              ].join(" ");
              // console.log("d", d);

              return d;       
          }
          // svg.append("path")
          //   // todo
          //   .attr("stroke-width", 1)
          //   // todo
          //   .attr("fill", "none")
          //   // todo
          //   .attr("stroke", "#000000")
          //   .attr("d", describeArc(arc.origin.x, arc.origin.y, arc.radius, arc.angle_one - 0.001, arc.angle_two));
          console.log("todo: render arcs properly");

        } else if(command.is_path()) {
          const path = command.as_path();
          render_commands(path.commands);
          console.log("todo: implement fills for paths.");

        } else if(command.is_text()) {
          const text = command.as_text();
          lhasa_text_to_d3js(svg, text);
        }
      }
    };
    render_commands(commands);
    commands.delete();
    ren.delete();

    const elem = document.getElementById("editor_canvas_container");
    while (elem.firstChild) {
      elem.removeChild(elem.lastChild);
    }
    elem.append(svg.node());
  };
  return (
    <>
      <div id="lhasa_editor">
        <div id="lhasa_hello_outer" className="horizontal_container">
          <img src="/icons/icons/hicolor_apps_scalable_coot-layla.svg" />
          <div id="lhasa_hello">
            <h3>Welcome to Lhasa!</h3>
            <p>
              Lhasa is a WebAssemby port of Layla - Coot's Ligand Editor.<br/>
              Lhasa is experimental software.
            </p>
            <p>
              This is a demo UI for development purposes.
            </p>
          </div>
        </div>
        <div id="molecule_tools_toolbar" className="horizontal_toolbar toolbar horizontal_container">
          <ToolButton caption="Move" onclick={() => switch_tool(new Lhasa.TransformTool(Lhasa.TransformMode.Translation))} />
          <ToolButton caption="Rotate" onclick={() => switch_tool(new Lhasa.TransformTool(Lhasa.TransformMode.Rotation))} />
          <ToolButton caption="Flip around X" onclick={() => switch_tool(new Lhasa.FlipTool(Lhasa.FlipMode.Horizontal))} />
          <ToolButton caption="Flip around Y" onclick={() => switch_tool(new Lhasa.FlipTool(Lhasa.FlipMode.Vertical))} />
          <ToolButton caption="Delete hydrogens" onclick={() => switch_tool(new Lhasa.RemoveHydrogensTool())} />
          <ToolButton caption="Format" onclick={() => switch_tool(new Lhasa.FormatTool())} />
        </div>
        <div id="main_tools_toolbar" className="horizontal_toolbar toolbar horizontal_container">
          <ToolButton caption="Single Bond" onclick={() => switch_tool(new Lhasa.BondModifier(Lhasa.BondModifierMode.Single))} />
          <ToolButton caption="Double Bond" onclick={() => switch_tool(new Lhasa.BondModifier(Lhasa.BondModifierMode.Double))} />
          <ToolButton caption="Triple Bond" onclick={() => switch_tool(new Lhasa.BondModifier(Lhasa.BondModifierMode.Triple))} />
          <ToolButton caption="Geometry" onclick={() => switch_tool(new Lhasa.GeometryModifier())} />
          <ToolButton caption="Charge" onclick={() => switch_tool(new Lhasa.ChargeModifier())} icon="icons/layla_charge_tool.svg" />
          <ToolButton caption="Delete" onclick={() => switch_tool(new Lhasa.DeleteTool())} />
        </div>
        <div id="structure_toolbar" className="horizontal_toolbar toolbar horizontal_container">
          <ToolButton caption="3-C" onclick={() => switch_tool(new Lhasa.StructureInsertion(Lhasa.Structure.CycloPropaneRing))} />
          <ToolButton caption="4-C" onclick={() => switch_tool(new Lhasa.StructureInsertion(Lhasa.Structure.CycloButaneRing))} />
          <ToolButton caption="5-C" onclick={() => switch_tool(new Lhasa.StructureInsertion(Lhasa.Structure.CycloPentaneRing))} />
          <ToolButton caption="6-C" onclick={() => switch_tool(new Lhasa.StructureInsertion(Lhasa.Structure.CycloHexaneRing))} />
          <ToolButton caption="6-Arom" onclick={() => switch_tool(new Lhasa.StructureInsertion(Lhasa.Structure.BenzeneRing))} />
          <ToolButton caption="7-C" onclick={() => switch_tool(new Lhasa.StructureInsertion(Lhasa.Structure.CycloHeptaneRing))} />
          <ToolButton caption="8-C" onclick={() => switch_tool(new Lhasa.StructureInsertion(Lhasa.Structure.CycloOctaneRing))} />
        </div>
        <div id="x_element_panel" className="panel horizontal_container" >
          {/* <span style="align-self: center;flex-grow:1;">Custom element symbol: </span> */}
          <input id="x_element_symbol_input"></input>
          {/* <div className="button" id="x_element_submit_button" onclick="javascript:on_x_element_submit_button(this)">Submit</div> */}
        </div>
        <div id="error_display" className="vertical_container vertical_toolbar">

        </div>
        <div id="main_horizontal_container" className="horizontal_container">
          <div id="element_toolbar" className="vertical_toolbar toolbar vertical_container">
            <ToolButton caption="C" onclick={() => switch_tool(new Lhasa.ElementInsertion(Lhasa.Element.C))} />
            <ToolButton caption="N" onclick={() => switch_tool(new Lhasa.ElementInsertion(Lhasa.Element.N))} />
            <ToolButton caption="O" onclick={() => switch_tool(new Lhasa.ElementInsertion(Lhasa.Element.O))} />
            <ToolButton caption="S" onclick={() => switch_tool(new Lhasa.ElementInsertion(Lhasa.Element.S))} />
            <ToolButton caption="P" onclick={() => switch_tool(new Lhasa.ElementInsertion(Lhasa.Element.P))} />
            <ToolButton caption="H" onclick={() => switch_tool(new Lhasa.ElementInsertion(Lhasa.Element.H))} />
            <ToolButton caption="F" onclick={() => switch_tool(new Lhasa.ElementInsertion(Lhasa.Element.F))} />
            <ToolButton caption="Cl" onclick={() => switch_tool(new Lhasa.ElementInsertion(Lhasa.Element.Cl))} />
            <ToolButton caption="Br" onclick={() => switch_tool(new Lhasa.ElementInsertion(Lhasa.Element.Br))} />
            <ToolButton caption="I" onclick={() => switch_tool(new Lhasa.ElementInsertion(Lhasa.Element.I))} />
            <ToolButton caption="X" id="x_element_button" />
          </div>
          <div id="editor_canvas_container">
            <div id="pre_render_message">Lhasa not loaded.</div>
          </div>
        </div>
        <div id="status_display_panel" className="panel">
          <span>▶</span>
          <span id="status_display"></span>
        </div>
        <div id="invalid_molecules_panel" className="panel">
          {/* <input 
            type="checkbox" 
            id="allow_invalid_molecules_checkbox" 
            name="allow_invalid_molecules"
            onChange={() => chLh(() => lh.set_allow_invalid_molecules(this.checked))}
            >Allow invalid molecules</input> */}
        </div>
        <div id="info_block" className="horizontal_container">
          <div id="scale_panel" className="panel">
            <b>SCALE</b>
            <div id="scale_display">
              {/* <!-- scale info goes here --> */}
            </div>
            <div className="toolbar horizontal_toolbar horizontal_container">
              {/* <div className="button" onclick="javascript:{const sc = lh.get_scale(); lh.set_scale(sc-0.05);}"><b>-</b></div>
              <div className="button" onclick="javascript:{const sc = lh.get_scale(); lh.set_scale(sc+0.05);}"><b>+</b></div> */}
            </div>
          </div>
          <div id="display_mode_panel" className="panel">
            <b>DISPLAY MODE</b>
            {/* <br/><input type="radio" name="display_mode" checked value="standard" onchange="javascript:switch_display_mode(this.value);">Standard</input>
            <br/><input type="radio" name="display_mode" value="atom_indices" onchange="javascript:switch_display_mode(this.value);">Atom Indices</input> */}
            {/* <!-- <input type="radio" name="display_mode" id="display_mode_atom_names">Atom Names</input> --> */}
          </div>
          <div id="smiles_display_outer" className="panel">
            <b>SMILES</b>
            <div id="smiles_display">
              {/* <!-- Smiles goes here --> */}
            </div>
          </div>
        </div>
        <div id="bottom_toolbar" className="horizontal_toolbar toolbar horizontal_container">
          {/* <div className="button" onclick="javascript:lh.undo_edition()" >Undo</div>
          <div className="button" onclick="javascript:lh.redo_edition()" >Redo</div> */}
          {/* <div style="flex-grow: 1;" className="horizontal_container toolbar">
            <input id="smiles_input"></input>
            <div className="button" id="smiles_import_button">Import SMILES</div>
          </div> */}
        </div>
        <div id="lhasa_footer">
          <i>Written by Jakub Smulski</i>
        </div>
      </div>
    </>
  )
}

export function App() {
  // const [count, setCount] = useState(0)

  return (
    <>
     <LhasaComponent />
    </>
  )
}

// export default App
