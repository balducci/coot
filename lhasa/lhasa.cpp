#include "lhasa.hpp"
#include <rdkit/GraphMol/SmilesParse/SmilesParse.h>
#include <rdkit/GraphMol/SmilesParse/SmilesWrite.h>
#include "glog_replacement.hpp"

std::unique_ptr<RDKit::RWMol> lhasa::rdkit_mol_from_smiles(std::string smiles) {
    std::unique_ptr<RDKit::RWMol> ret(RDKit::SmilesToMol(smiles));
    return ret;
}

std::string lhasa::rdkit_mol_to_smiles(RDKit::ROMol& mol) {
    auto ret = RDKit::MolToSmiles(mol);
    return ret;
}

void lhasa::append_from_smiles(CootLigandEditorCanvas& canvas, std::string smiles) {
    canvas.append_molecule(rdkit_mol_from_smiles(smiles));
}

std::unique_ptr<coot::ligand_editor_canvas::ActiveTool> lhasa::make_active_tool(emscripten::val tool) {
    using namespace coot::ligand_editor_canvas;

    // Just returns 'object'
    //std::string type_name = tool.typeOf().as<std::string>();

    std::string classname = tool["__proto__"]["constructor"]["name"].as<std::string>();
    // std::cout<< classname << '\n';

    if(classname == "ElementInsertion") {
        return std::make_unique<ActiveTool>(tool.as<ElementInsertion>());
    }
    if(classname == "BondModifier") {
        return std::make_unique<ActiveTool>(tool.as<BondModifier>());
    }
    if(classname == "TransformTool") {
        return std::make_unique<ActiveTool>(tool.as<TransformTool>());
    }
    if(classname == "StructureInsertion") {
        return std::make_unique<ActiveTool>(tool.as<StructureInsertion>());
    }
    if(classname == "FlipTool") {
        return std::make_unique<ActiveTool>(tool.as<FlipTool>());
    }
    if(classname == "DeleteTool") {
        return std::make_unique<ActiveTool>(tool.as<DeleteTool>());
    }
    if(classname == "ChargeModifier") {
        return std::make_unique<ActiveTool>(tool.as<ChargeModifier>());
    }
    if(classname == "GeometryModifier") {
        return std::make_unique<ActiveTool>(tool.as<GeometryModifier>());
    }
    if(classname == "FormatTool") {
        return std::make_unique<ActiveTool>(tool.as<FormatTool>());
    }
    if(classname == "RemoveHydrogensTool") {
        return std::make_unique<ActiveTool>(tool.as<RemoveHydrogensTool>());
    }

    g_critical("%s does not correspond to any known tool type. Returning empty ActiveTool.", classname.c_str());
    return std::make_unique<ActiveTool>();

}

coot::ligand_editor_canvas::ElementInsertion lhasa::element_insertion_from_symbol(std::string sym) {
    return coot::ligand_editor_canvas::ElementInsertion(sym.c_str());
}