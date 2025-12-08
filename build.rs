use std::env;

fn main() {
    let path = env::var_os("PLUGIN_DEFAULT_SEARCH_PATH")
        .map(|s| s.into_string())
        .transpose()
        .expect("PLUGIN_DEFAULT_SEARCH_PATH contains non‑UTF‑8 data")
        .unwrap_or_else(|| {
            env::current_dir()
                .expect("failed to obtain current working directory")
                .join("./script")
                .to_string_lossy()
                .into_owned()
        });

    println!("cargo:rustc-env=PLUGIN_DEFAULT_SEARCH_PATH={}", path);
}
