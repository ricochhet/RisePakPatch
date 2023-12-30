use std::env;

use rise_pak_patch::patcher::RisePakPatch;

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let args: Vec<String> = env::args().collect();

    if args.len() != 4 {
        return Ok(());
    }

    let arg1 = &args[1];
    let arg2 = &args[2];
    let arg3: &String = &args[3];
    let mut patch = RisePakPatch::new();

    match arg1.as_str() {
        "patch" => patch.process_directory(arg2, arg3, false)?,
        "patch2" => patch.process_directory(arg2, arg3, true)?,
        "extract" => patch.extract_directory(arg2, arg3, false)?,
        "extract2" => patch.extract_directory(arg2, arg3, true)?,
        _ => {
            return Ok(());
        }
    }

    Ok(())
}