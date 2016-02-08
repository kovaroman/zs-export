<?hh // strict
namespace ElasticExport\Generators;

use ElasticExport\Helper\ItemExportHelper;
use Plenty\Modules\DataExchange\Contracts\CSVGenerator;
use Plenty\Modules\DataExchange\Models\FormatSetting;

/**
 * Class IdealoGenerator
 * @package ElasticExport\Generators
 */
class IdealoGenerator extends CSVGenerator
{
    /*
     * @var
     */
    private ItemExportHelper $itemExportHelper;

    /**
     * IdealoGenerator constructor.
     * @param ItemExportHelper $itemExportHelper
     */
    public function __construct(ItemExportHelper $itemExportHelper)
    {
        $this->itemExportHelper = $itemExportHelper;
    }

    protected function generateContent(mixed $resultData, array<FormatSetting> $formatSettings = []):void
    {
        //code here
    }
}
