<?hh // strict
namespace ElasticExport\Generators;

use ElasticExport\Helper\ElasticExportHelper;
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
    private ElasticExportHelper $elasticExportHelper;

    /**
     * IdealoGenerator constructor.
     * @param ElasticExportHelper $elasticExportHelper
     */
    public function __construct(ElasticExportHelper $elasticExportHelper)
    {
        $this->elasticExportHelper = $elasticExportHelper;
    }

    protected function generateContent(mixed $resultData, array<FormatSetting> $formatSettings = []):void
    {
        //code here
    }
}
