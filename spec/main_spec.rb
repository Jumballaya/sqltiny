##
# SQLTiny Tests
##

describe 'database' do
  def run_script(cmds)
    raw = nil
    IO.popen("./bin/sqltiny", "r+") do |pipe|
      cmds.each do |cmd|
        pipe.puts cmd
      end

      pipe.close_write

      raw = pipe.gets(nil)
    end
    raw.split("\n")
  end

  it 'inserts and retreives a row' do
    res = run_script([
      "insert 1 usera user@a.com",
      "select",
      ".exit",
    ])
    expect(res).to match_array([
      "> Executed",
      "> (1, usera, user@a.com)",
      "Executed",
      "> ",
    ])
  end

  it 'prints error message when table is full' do
    script = (1..1401).map do |i|
      "insert #{i} user#{i} person#{i}@example.com"
    end
    script << ".exit"
    res = run_script(script)
    expect(res[-2]).to eq('> error: Table full.')
  end

  it 'allows inserting strings that are the maximum length' do
    long_username = "a"*255;
    long_email = "a"*255;
    script = [
      "insert 1 #{long_username} #{long_email}",
      "select",
      ".exit",
    ]
    res = run_script(script)
    expect(res).to match_array([
      "> Executed",
      "> (1, #{long_username}, #{long_email})",
      "Executed",
      "> ",
    ])
  end

  it 'prints error message if strings are too long' do
    long_username = "a"*256;
    long_email = "a"*256;
    script = [
      "insert 1 #{long_username} #{long_email}",
      "select",
      ".exit",
    ]
    res = run_script(script)
    expect(res).to match_array([
      "> error: string is too long"
      "Executed",
      "> ",
    ])
  end

end
